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