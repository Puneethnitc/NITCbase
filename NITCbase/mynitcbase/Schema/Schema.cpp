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

int Schema::createIndex(char relName[ATTR_SIZE],char attrName[ATTR_SIZE]){
    // if the relName is either Relation Catalog or Attribute Catalog,
        // return E_NOTPERMITTED
        // (check if the relation names are either "RELATIONCAT" and "ATTRIBUTECAT".
        // you may use the following constants: RELCAT_NAME and ATTRCAT_NAME)
	if (
		strcmp(relName, RELCAT_RELNAME) == 0 ||
		strcmp(relName, ATTRCAT_RELNAME) == 0
	) {
		return E_NOTPERMITTED;
	}

    // get the relation's rel-id using OpenRelTable::getRelId() method
	int relId = OpenRelTable::getRelId(relName);

    // if relation is not open in open relation table, return E_RELNOTOPEN
    // (check if the value returned from getRelId function call = E_RELNOTOPEN)
	if (relId == E_RELNOTOPEN) {
		return E_RELNOTOPEN;
	}

    // create a bplus tree using BPlusTree::bPlusCreate() and return the value
    return BPlusTree::bPlusCreate(relId, attrName);
}

/* This method drops the bplus indexing on an attribute attrName in a relation relName as specified in arguments. */
int Schema::dropIndex(char *relName, char *attrName) {
    // if the relName is either Relation Catalog or Attribute Catalog,
        // return E_NOTPERMITTED
        // (check if the relation names are either "RELATIONCAT" and "ATTRIBUTECAT".
        // you may use the following constants: RELCAT_NAME and ATTRCAT_NAME)
	if (
		strcmp(relName, RELCAT_RELNAME) == 0 ||
		strcmp(relName, ATTRCAT_RELNAME) == 0
	) {
		return E_NOTPERMITTED;
	}

    // get the relation's rel-id using OpenRelTable::getRelId() method
	int relId = OpenRelTable::getRelId(relName);

    // if relation is not open in open relation table, return E_RELNOTOPEN
    // (check if the value returned from getRelId function call = E_RELNOTOPEN)
	if (relId == E_RELNOTOPEN) {
		return E_RELNOTOPEN;
	}

    // get the attribute catalog entry corresponding to the attribute
    // using AttrCacheTable::getAttrCatEntry()
	AttrCatEntry attrCatEntry;
	int retVal = AttrCacheTable::getAttrCatEntry(relId, attrName, &attrCatEntry);

    // if getAttrCatEntry() fails, return E_ATTRNOTEXIST
	if (retVal == E_ATTRNOTEXIST) {
		return E_ATTRNOTEXIST;
	}

	/* get the root block from attrcat entry */
    int rootBlock = attrCatEntry.rootBlock;

	/* if attribute does not have an index (rootBlock = -1) */
    if (rootBlock == -1) {
        return E_NOINDEX;
    }

    // destroy the bplus tree rooted at rootBlock using BPlusTree::bPlusDestroy()
    BPlusTree::bPlusDestroy(rootBlock);

    // set rootBlock = -1 in the attribute cache entry of the attribute using
    // AttrCacheTable::setAttrCatEntry()
	attrCatEntry.rootBlock = -1;
	AttrCacheTable::setAttrCatEntry(relId, attrName, &attrCatEntry);

    return SUCCESS;
}