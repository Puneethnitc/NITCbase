#include "Algebra.h"

#include <cstring>
#include<stdio.h>
#include<cstdlib>
/* used to select all the records that satisfy a condition.
the arguments of the function are
- srcRel - the source relation we want to select from
- targetRel - the relation we want to select into. (ignore for now)
- attr - the attribute that the condition is checking
- op - the operator of the condition
- strVal - the value that we want to compare against (represented as a string)
*/
bool isNumber(char *str);

int Algebra::select(char srcRel[ATTR_SIZE], char targetRel[ATTR_SIZE], char attr[ATTR_SIZE], int op, char strVal[ATTR_SIZE]) {
  int srcRelId = OpenRelTable::getRelId(srcRel);      // we'll implement this later
  if (srcRelId == E_RELNOTOPEN) {
    return E_RELNOTOPEN;
  }

  AttrCatEntry attrCatEntry;
  // get the attribute catalog entry for attr, using AttrCacheTable::getAttrcatEntry()
  //    return E_ATTRNOTEXIST if it returns the error
  
 int ret= AttrCacheTable::getAttrCatEntry(srcRelId,attr,&attrCatEntry);
 if(ret!=SUCCESS)
 return ret;
  /*** Convert strVal (string) to an attribute of data type NUMBER or STRING ***/
  int type = attrCatEntry.attrType;
  Attribute attrVal;
  if (type == NUMBER) {
    if (isNumber(strVal)) {       // the isNumber() function is implemented below
      attrVal.nVal = atof(strVal);
    } else {
      return E_ATTRTYPEMISMATCH;
    }
  } else if (type == STRING) {
    strcpy(attrVal.sVal, strVal);
  }

    /*** Creating and opening the target relation ***/
    // Prepare arguments for createRel() in the following way:
    // get RelcatEntry of srcRel using RelCacheTable::getRelCatEntry()
    RelCatEntry relCatEntry;
    RelCacheTable::getRelCatEntry(srcRelId,&relCatEntry);
    int src_nAttrs = relCatEntry.numAttrs;/* the no. of attributes present in src relation */ 
    char attr_names[src_nAttrs][ATTR_SIZE];
    int attr_types[src_nAttrs];

    for(int i=0;i<src_nAttrs;i++)
    { 
      AttrCatEntry attrCatBuf;
      AttrCacheTable::getAttrCatEntry(srcRelId,i,&attrCatBuf);
      strcpy(attr_names[i],attrCatBuf.attrName);
      attr_types[i]=attrCatBuf.attrType;
    }

    /* Create the relation for target relation by calling Schema::createRel()
       by providing appropriate arguments */
    // if the createRel returns an error code, then return that value.
   int retVal=Schema::createRel(targetRel,src_nAttrs,attr_names,attr_types);
   if(retVal!=SUCCESS)
   return retVal;

     /* Open the newly created target relation by calling OpenRelTable::openRel()
       method and store the target relid */
      retVal=OpenRelTable::openRel(targetRel);
      int targetRelId;
      if(retVal>=0){ 
       targetRelId=retVal;
      }else{
        Schema::deleteRel(targetRel);
        return retVal;
      }

      
      /* If opening fails, delete the target relation by calling Schema::deleteRel()
      and return the error value returned from openRel() */
      Attribute record[src_nAttrs];
       RelCacheTable::resetSearchIndex(srcRelId);
    // AttrCacheTable::resetSearchIndex(srcRelId,attr);
  /*** Selecting records from the source relation ***/

  while(BlockAccess::search(srcRelId,record,attr,attrVal,op)==SUCCESS)
  {
    ret=BlockAccess::insert(targetRelId,record);
    if(ret!=SUCCESS)
    {
      Schema::closeRel(targetRel);
      Schema::deleteRel(targetRel);
      return ret;
    }
  }
  Schema::closeRel(targetRel);
 return SUCCESS;
}


// will return if a string can be parsed as a floating point number
bool isNumber(char *str) {
  int len;
  float ignore;
  /*
    sscanf returns the number of elements read, so if there is no float matching
    the first %f, ret will be 0, else it'll be 1

    %n gets the number of characters read. this scanf sequence will read the
    first float ignoring all the whitespace before and after. and the number of
    characters read that far will be stored in len. if len == strlen(str), then
    the string only contains a float with/without whitespace. else, there's other
    characters.
  */
  int ret = sscanf(str, "%f %n", &ignore, &len);
  return ret == 1 && len == strlen(str);
}
int Algebra::insert(char relName[ATTR_SIZE],int nAttrs,char record[][ATTR_SIZE])
{
  if(strcmp(relName,RELCAT_RELNAME)==0)
  return E_NOTPERMITTED;
  if(strcmp(relName,ATTRCAT_RELNAME)==0)
  return E_NOTPERMITTED;

  int relId=OpenRelTable::getRelId(relName);
  if(relId==E_RELNOTOPEN)
  return relId;

  RelCatEntry relCatEntry;
  RelCacheTable::getRelCatEntry(relId,&relCatEntry);
  if(relCatEntry.numAttrs!=nAttrs)
  return E_NATTRMISMATCH;

  Attribute recordValues[nAttrs];

  for(int i=0;i<nAttrs;i++)
  { AttrCatEntry attrCatBuf;
    AttrCacheTable::getAttrCatEntry(relId,i,&attrCatBuf);
    int type=attrCatBuf.attrType;
    if(type==NUMBER)
    {
      if(isNumber(record[i]))
      {
        recordValues[i].nVal=atof(record[i]);
      }
      else
      return E_ATTRTYPEMISMATCH;
    }
    else if(type==STRING){
        strcpy(recordValues[i].sVal,record[i]);
    }
  }
  int retVal=BlockAccess::insert(relId,recordValues);
  return retVal;
}


int Algebra::project(char srcRel[ATTR_SIZE],char targetRel[ATTR_SIZE]){
  int srcRelId=OpenRelTable::getRelId(srcRel);
  if(srcRelId==E_RELNOTOPEN)
  return E_RELNOTOPEN;

  RelCatEntry relCatBufSrc;
  RelCacheTable::getRelCatEntry(srcRelId,&relCatBufSrc);
  int numAttrsSrc=relCatBufSrc.numAttrs;

  char attrNames[numAttrsSrc][ATTR_SIZE];
  int attrTypes[numAttrsSrc];

  for(int i=0;i<numAttrsSrc;i++)
  { 
    AttrCatEntry attrCatBuf;
    AttrCacheTable::getAttrCatEntry(srcRelId,i,&attrCatBuf);
    strcpy(attrNames[i],attrCatBuf.attrName);
    attrTypes[i]=attrCatBuf.attrType;
  }

 int retVal=Schema::createRel(targetRel,numAttrsSrc,attrNames,attrTypes);
  if(retVal!=SUCCESS)
  return retVal;

  int targetRelId=OpenRelTable::openRel(targetRel);
  if(targetRelId<0)
  {
    Schema::deleteRel(targetRel);
    return targetRelId;
  }

  RelCacheTable::resetSearchIndex(srcRelId);
  Attribute record[numAttrsSrc];
  while(BlockAccess::project(srcRelId,record)==SUCCESS)
  {
    retVal=BlockAccess::insert(targetRelId,record);
    if(retVal!=SUCCESS)
    {
      Schema::closeRel(targetRel);
      Schema::deleteRel(targetRel);
      return retVal;
    }
  }
  Schema::closeRel(targetRel);
  return SUCCESS;
} 

int Algebra::project(char srcRel[ATTR_SIZE],char targetRel[ATTR_SIZE],int tar_nAttrs,char tar_Attrs[][ATTR_SIZE])
{
    int srcRelId=OpenRelTable::getRelId(srcRel);
    if(srcRelId==E_RELNOTOPEN)
    {
      return E_RELNOTOPEN;
    }

    RelCatEntry srcRelCatEntry;
    RelCacheTable::getRelCatEntry(srcRelId,&srcRelCatEntry);
    int srcNumOfAttrs=srcRelCatEntry.numAttrs;

    int attr_off[tar_nAttrs];
    int attr_types[tar_nAttrs];

    for(int i=0;i<tar_nAttrs;i++)
    {
        AttrCatEntry attrCatEntry;
      int retVal =AttrCacheTable::getAttrCatEntry(srcRelId,tar_Attrs[i],&attrCatEntry);
      if(retVal!=SUCCESS)
      return retVal;
      else{
        attr_off[i]=attrCatEntry.offset;
        attr_types[i]=attrCatEntry.attrType;
      }
    }

    int retVal=Schema::createRel(targetRel,tar_nAttrs,tar_Attrs,attr_types);
    if(retVal!=SUCCESS)
    return retVal;

    int tarRelId=OpenRelTable::openRel(targetRel);
    if(tarRelId<0){ 
      Schema::deleteRel(targetRel);
    return tarRelId;
    }

    RelCacheTable::resetSearchIndex(srcRelId);
    Attribute record[srcNumOfAttrs];

    while(BlockAccess::project(srcRelId,record)==SUCCESS)
    {
      Attribute proj_record[tar_nAttrs];
      for(int i=0;i<tar_nAttrs;i++)
      proj_record[i]=record[attr_off[i]];
          retVal=BlockAccess::insert(tarRelId,proj_record);
          if(retVal!=SUCCESS)
          {
            Schema::closeRel(targetRel);
            Schema::deleteRel(targetRel);
            return retVal;
          }
        
    }
    Schema::closeRel(targetRel);
    return SUCCESS;
}