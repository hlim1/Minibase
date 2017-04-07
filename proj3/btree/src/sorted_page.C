/*
 * sorted_page.C - implementation of class SortedPage
 *
 * Johannes Gehrke & Gideon Glass  951016  CS564  UW-Madison
 * Edited by Young-K. Suh (yksuh@cs.arizona.edu) 03/27/14 CS560 Database Systems Implementation 
 */

#include <vector>
#include <algorithm>
#include <cstring>

#include "sorted_page.h"
#include "btindex_page.h"
#include "btleaf_page.h"


class SlotData
{
   public:
    char *data;
    int slotNo;
    int length;
    int offset;
    SlotData();
    bool operator<(const SlotData &other) const{
        return strcmp(data,other.data) < 0;
    }
};

SlotData::SlotData() {}


const char* SortedPage::Errors[SortedPage::NR_ERRORS] = {
  //OK,
  //Insert Record Failed (SortedPage::insertRecord),
  //Delete Record Failed (SortedPage::deleteRecord,
};


/*
 *  Status SortedPage::insertRecord(AttrType key_type, 
 *                                  char *recPtr,
 *                                    int recLen, RID& rid)
 *
 * Performs a sorted insertion of a record on an record page. The records are
 * sorted in increasing key order.
 * Only the  slot  directory is  rearranged.  The  data records remain in
 * the same positions on the  page.
 *  Parameters:
 *    o key_type - the type of the key.
 *    o recPtr points to the actual record that will be placed on the page
 *            (So, recPtr is the combination of the key and the other data
 *       value(s)).
 *    o recLen is the length of the record to be inserted.
 *    o rid is the record id of the record inserted.
 */

Status SortedPage::insertRecord (AttrType key_type,
                                 char * recPtr,
                                 int recLen,
                                 RID& rid)
{
  // put your code here

  if(key_type !=type)
    return DONE;
  Status status = HFPage::insertRecord(recPtr,recLen,rid);

  if(status == OK && this->slotCnt==0)
    return OK;

  slot_t *current = this->slot;
  int i=0;
  vector<SlotData> slotsInfo;

  while(i <= this->slotCnt)
  {
      SlotData slotData;
      slotData.slotNo = i;

      int offset = current->offset;
      int length = current->length;
      slotData.length = length;
      slotData.offset = offset;
      slotData.data = data+offset; // have to fix this, should only store key
      slotsInfo.push_back(slotData);
      current = ((slot_t *)data+i*sizeof(slot_t));
      i++;
  }
  std::sort(slotsInfo.begin(),slotsInfo.end());

  for(int i=0;i<slotsInfo.size();i++)
  {
        int slotNo = slotsInfo[i].slotNo;
        if(slotNo == 0)
        {
            this->slot->length = slotsInfo[i].length;
            this->slot->offset = slotsInfo[i].offset;
        } else
        {
            slot_t *temp = (slot_t *)data+(slotNo-1)*sizeof(slot_t);
            temp->offset = slotsInfo[i].length;
            temp->length = slotsInfo[i].offset;
        }
  }
  return OK;
}


/*
 * Status SortedPage::deleteRecord (const RID& rid)
 *
 * Deletes a record from a sorted record page. It just calls
 * HFPage::deleteRecord().
 */

Status SortedPage::deleteRecord (const RID& rid)
{
  // put your code here
    Status status =HFPage::deleteRecord(rid);
    if(status!=OK)
        return DONE;


  return status;
}

int SortedPage::numberOfRecords()
{
  // put your code here
  int i=0;
  slot_t *current = this->slot;
  int numRecords = 0;
  while(i <= this->slotCnt)
  {
      if(current->offset!=-1)
          numRecords++;
      current = (slot_t*)data + i*sizeof(slot_t);
      i++;
  }
  return numRecords;
}