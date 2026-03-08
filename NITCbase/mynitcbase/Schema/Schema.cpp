#include "Schema.h"

#include <cmath>
#include <cstring>

int Schema::openRel(char relName[ATTR_SIZE])
{
    int ret=OpenRelTable::openRel(relName);

    if(ret>=0)
    return SUCCESS;

    return ret;
}

int Schema::closeRel(char relName[ATTR_SIZE])
{
   if((strcmp(relName,RELCAT_RELNAME)==0)||(strcmp(relName,ATTRCAT_RELNAME)==0))
   return E_NOTPERMITTED;

   int relId=OpenRelTable::getRelId(relName);
   if(relId==E_RELNOTOPEN)
   return E_RELNOTOPEN;

   return OpenRelTable::closeRel(relId);
}

int Schema::renameRel(char oldrelName[ATTR_SIZE],char newrelName[ATTR_SIZE])
{
    if((strcmp(oldrelName,RELCAT_RELNAME)==0)||(strcmp(oldrelName,ATTRCAT_RELNAME)==0))
    return E_NOTPERMITTED;

    if((strcmp(newrelName,RELCAT_RELNAME)==0)||(strcmp(newrelName,ATTRCAT_RELNAME)==0))
    return E_NOTPERMITTED;

    int ret=OpenRelTable::getRelId(oldrelName);
    if(ret!=E_RELNOTOPEN)
    return E_RELOPEN;

   
    return BlockAccess::renameRelation(oldrelName,newrelName);
       
}

int Schema::renameAttr(char relname[ATTR_SIZE],char oldattrName[ATTR_SIZE],char newattrName[ATTR_SIZE])
{
    if((strcmp(relname,RELCAT_RELNAME)==0)||(strcmp(relname,ATTRCAT_RELNAME)==0))
    return E_NOTPERMITTED;

    int ret=OpenRelTable::getRelId(relname);
    if(ret!=E_RELNOTOPEN)
    return E_RELOPEN;

    ret=BlockAccess::renameAttribute(relname,oldattrName,newattrName);
    return ret;
    
}

int Schema::createRel(char relName[ATTR_SIZE],int numOfAttrs,char attrNames[][ATTR_SIZE],int attrType[])
{
    Attribute relNameAttr;
    strcpy(relNameAttr.sVal,relName);

    RecId targetRecId;
    RelCacheTable::resetSearchIndex(RELCAT_RELID);
    targetRecId=BlockAccess::linearSearch(RELCAT_RELID,RELCAT_ATTR_RELNAME,relNameAttr,EQ);
    if(targetRecId.block!=-1&&targetRecId.slot!=-1)
    return E_RELEXIST;

    for(int i=0;i<numOfAttrs;i++)
    {
        for(int j=i+1;j<numOfAttrs;j++)
        {
            if(strcmp(attrNames[i],attrNames[j])==0)
            return E_DUPLICATEATTR;
        }
    }

    Attribute relCatRecord[RELCAT_NO_ATTRS];
    strcpy(relCatRecord[RELCAT_REL_NAME_INDEX].sVal,relName);
    relCatRecord[RELCAT_NO_ATTRIBUTES_INDEX].nVal=numOfAttrs;
    relCatRecord[RELCAT_NO_RECORDS_INDEX].nVal=0;
    relCatRecord[RELCAT_FIRST_BLOCK_INDEX].nVal=-1;
    relCatRecord[RELCAT_LAST_BLOCK_INDEX].nVal=-1;
    relCatRecord[RELCAT_NO_SLOTS_PER_BLOCK_INDEX].nVal=floor((2016/(16*numOfAttrs+1)));

    int retVal=BlockAccess::insert(RELCAT_RELID,relCatRecord);
    if(retVal!=SUCCESS)
    return retVal;

    for(int i=0;i<numOfAttrs;i++)
    {
        Attribute attrCatRecord[ATTRCAT_NO_ATTRS];

        strcpy(attrCatRecord[ATTRCAT_REL_NAME_INDEX].sVal,relName);
        strcpy(attrCatRecord[ATTRCAT_ATTR_NAME_INDEX].sVal,attrNames[i]);
        attrCatRecord[ATTRCAT_ATTR_TYPE_INDEX].nVal=attrType[i];
        attrCatRecord[ATTRCAT_PRIMARY_FLAG_INDEX].nVal=-1;
        attrCatRecord[ATTRCAT_ROOT_BLOCK_INDEX].nVal=-1;;
        attrCatRecord[ATTRCAT_OFFSET_INDEX].nVal=i;

        retVal=BlockAccess::insert(ATTRCAT_RELID,attrCatRecord);
        if(retVal==E_DISKFULL)
        {
            deleteRel(relName);
            return E_DISKFULL;
        }

    }
    return SUCCESS;
}

int Schema::deleteRel(char relName[ATTR_SIZE])
{
    if(strcmp(relName,RELCAT_RELNAME)==0)
    return E_NOTPERMITTED;
    if(strcmp(relName,ATTRCAT_RELNAME)==0)
    return E_NOTPERMITTED;

    int relId;
    relId=OpenRelTable::getRelId(relName);
    if(relId!=E_RELNOTOPEN)
    {
        return E_RELOPEN;
    }

   int retVal= BlockAccess::deleteRelation(relName);
    return retVal;
}