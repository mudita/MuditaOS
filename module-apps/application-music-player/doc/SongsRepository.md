# Songs Repository

In order for the music player to work properly, especially asking for the next and previous track from the list, we have to internally keep the list of currently processed tracks. 

## Music list cache memory

The cache for musicFiles contains a list of songs currently being processed. It is always updated in such a way that it is possible to get the next and previous track.

## Sequence flow 

![](./Images/update_data_sequence_flow.svg)

## UpdateRepository

**Queries:**

![](./Images/UpdateRepoQueries.svg)

**Receiving new records:**

1. newData
     - when new data is received, the musicFiles cache is completely overwritten 

![](./Images/newData.svg)

2. newBackData
     - when we receive newBackData, new records are added to the musicFiles cache and records from the top of the list are deleted 

![](./Images/newBackData.svg)

3. newFrontData
     - when we receive newFrontData, new records are added at the beginning of the musicFiles cache and records from the end of the list are deleted 

![](./Images/newFrontData.svg)

## Sort/Repeat Mode

  - The repeat mode does not affect the list of songs downloaded by the view (ListViewEngine), so the list view is sorted only by the sort mode.
  - It is different for the model: each Music Files Cache update must follow the logic shown in the diagram below.
  - Each change of sort / repeat mode must update the data according to this scheme.
  - For the shuffle mode, random songs are downloaded to the music files cache, therefore getting the next and previous track is done in the same way as for the other modes.

![](./Images/UpdateRepoSortRepeatModes.svg)
