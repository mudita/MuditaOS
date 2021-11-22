# SDD-0005-OS-User-Database

## Table of contents
1. [History](#history)
2. [Scope](#scope)
3. [Abbreviations](#abbreviations)
4. [Introduction](#introduction)
5. [Design considerations](#design)
6. [Database tables](#database-tables)
7. [Database triggers](#database-triggers)
8. [SMtask service & DBUS communication](#smtask-service--dbus-communication)
9. [References](#references)

## History

| Authors           | Change description        | Status | Modification date |
| ----------------- | ------------------------- | ------ | ----------------- |
| Łukasz Skrzypczak | Initial version           | Draft  | 27.08.2018        |
| Robert Borzęcki | Revision           | Draft  | 03.10.2018        |
| Robert Borzęcki | Review comments           | Draft  | 04.10.2018        |
| Robert Borzęcki | templates and settings tables          | Draft  | 10.01.2019        |
| Robert Borzęcki | alarms         | Draft  | 05.03.2019        |
| Kuba Kleczkowski | Added date_format in settings.db         | Draft  | 04.02.2020 |
| Alek Rudnik | calllog | Draft | 28.02.2020 |
| Paweł Olejniczak | Contact address | Draft | 07.05.2020 |

## Scope
This document is defines how SMS and contacts databases are constructed.

## Abbreviations

Abbreviation | Detailed explanation of abbreviation
------------|------------------------------------
(u) | unique
(m) | mandatory
(o) | optional
(r) | reference (other DB table relation)
<type> | In SQL queries <type> has to be replaced with proper data

### Introduction
This document describes details of different user information database tables:

| table name | description |
| ------------------- | --------------------|
| threads | Table that allows aggregation of messages sent/received to/from single number. |
| sms | Table responsible for holding content of the message with set of information needed for presenting messages in a thread view. |
| contacts | Aggregation table responsible for joining information from other more detailed contact tables. |
| contact_names | Tables responsible for holding information about name, surname and optional alternative display name for a given contact. |
| contact_numbers | Table that holds records with information about numbers assigned to given contact. Records in this table also allows defining type of the number (support for grouping). |
| contact_rings | This table holds name of the audio file that should be played when phone receives call from given number. |
| contact_address | Table that holds records with information about addresses assigned to given contact. |
| preferences | (TBD) |
| contact_groups | Dictionary defining groups for contact. |
| contact_match_group | Joining table for connecting users with groups. |


### Design
#### Database tables

#### 1. Contacts joining table
Name: contacts

| Field | Scope | Type | Description |
| -------- | ----------- | ------- | -------------------|
| _id | (um) | INTEGER PRIMARY KEY | Unique number assigned to the contact's joining record. |
| name_id | (mr) | INTEGER FOREIGN KEY(**contact_name**) | Unique ID in the contact_names table where display name for given contact is stored. |
| numbers_id | (mr) | TEXT | List of unique IDs assigned to records where phone numbers assigned to given contact are stored. Values are separated with spaces. |
| ring_id | (m) | INTEGER FOREIGN KEY(**contact_ring**)| Unique ID of the record in the contact_rings table where path to audio asset selected for playing during given contact's call |
| address_id | (m) | INTEGER FOREIGN KEY(**contact_address**) | Unique ID in the contact_address table where address assigned to given contact is stored. |
| type | (m) | INTEGER | Type of the contact as defined in the table below.  |

Possible values of the type field in the contacts' table

| Name | Value |  Description |
| -------- | ------- | -------------------|
| USER| 0| ID defines that contact was created by the user and should be displayed in the applications that manipulate contacts. | 
| TEMPORARY | 1| ID defines that contact was created for unknown messager/caller purposes. Possible reasons are: user created draft of the message, user received a message/call from unknown number |

#### 2. Contacts name table
Name: contact_names

| Field | Scope | Type | Description |
| -------- | ----------- | ------- | -------------------|
| _id | (um) | INTEGER PRIMARY KEY | Unique number assigned to the contact's name information. |
| contact_id | (um) | INTEGER FOREIGN KEY(**contacts**)| Unique ID of the joining record in the contacts table.  |
| name_primary | (m) | TEXT | Name  |
| name_alternative | (m) | TEXT | Alternative name for user. |

#### 3. Contacts number table
Name: contact_numbers

| Field | Scope | Type | Description |
| -------- | ----------- | ------- | -------------------|
| _id | (um) | INTEGER PRIMARY KEY | Unique number assigned to the contact's number information. |
| contact_id | (um) | INTEGER FOREIGN KEY(**contacts**)| Unique ID of the joining record in the contacts table.  |
| number_user | (m) | TEXT | Phone number entered by the user |
| number_e164 | (m) | TEXT | Phone number in the E164 format created from the number entered by the user . |
| type | (m) | INTEGER | Type of the number provided by the user. Default value is 0 (CELL). |

Possible values of the type field

| Name | Value |  Description |
| -------- | ------- | -------------------|
| CELL| 0| ID defines that number is a cellphone number. | 
| HOME| 1| ID defines that number is a home number. |
| WORK| 2| ID defines that number belongs to predefined work number's group.|
| FAKS  | 3| ID defines that number if a faks number.|
| PAGER| 4 | ID defines that number is a pager number|
| OTHER| 5| ID defines that number is for device other than defined above. |

#### 4. Contacts ring assets table
Name: contact_rings

| Field | Scope | Type | Description |
| -------- | ----------- | ------- | -------------------|
| _id | (um) | INTEGER PRIMARY KEY | Unique number assigned to the contact's name information. |
| contact_id | (um) | INTEGER FOREIGN KEY(**contacts**) | Unique ID of the joining record in the contacts table.  |
| asset_path | (m) | TEXT | UTF8 encoded path on the device's memory card to the audio asset that should be played when given contact is calling |

#### 5. Contacts address table
Name: contact_address

| Field | Scope | Type | Description |
| -------- | ----------- | ------- | -------------------|
| _id | (um) | INTEGER PRIMARY KEY | Unique number assigned to the contact's address information. |
| contact_id | (um) | INTEGER FOREIGN KEY(**contacts**) | Unique ID of the joining record in the contacts table.  |
| address | (m) | TEXT | Address information provided by the user. |
| note | (m) | TEXT | Optional note about given address provided by the user. |


Possible values of the address type field

| Name | Value |  Description |
| -------- | ------- | -------------------|
| HOME| 0| ID defines that address is home address. |
| WORK| 1| ID defines that address is work address. |
| OTHER| 2| ID defines that address doesn't belong to any of the types defined above. |

#### 6. SMS messages table
Name: sms

| Field | Scope | Type | Description |
| -------- | ----------- | ------- | -------------------|
| _id | (um) | INTEGER PRIMARY KEY | Unique ID of the message. |
| thread_id | (um) | INTEGER FOREIGN KEY(**threads**)| Unique ID of the thread that this message bellongs to.  |
| contact_id | (m) | INTEGER | Unique ID of the record from the **contacts** table from *contacts** database where details about sender/recipient of the message are located. |
| date | (m) | INTEGER | Date when message was received |
| date_sent | (m) | INTEGER | Date when message was sent to recipients |
| error_code | (m) | INTEGER | ID of the error code for this message |
| body | (m) | TEXT | Text of the message that was provided by the user. This is UTF8 encoded text. |
| type | (m) | INTEGER | Defines what is the type of the message |

Possible values of the type field

| Name | Value |  Description |
| -------- | ------- | -------------------|
| DRAFT| 0x01| Defines unfinished message that should be loaded in editor for a given thread and may be later scheduled for sending. |
| FAILED| 0x02| Defines message that was queued for sending but this process for some reasons failed |
| INBOX| 0x04| Defines incoming message.|
| OUTBOX | 0x08| Defines outgoing message. |
| QUEUED| 0x10 | Defines message that is scheduled for sending|
| ALL| 0xFF| Defines all types of messages. |

NOTE: Date is designed to be stored in UNIX (EPOCH) format - number of seconds since 1970-01-01 00:00:00 UTC.

#### 7. SMS threads table
Name: threads

| Field | Scope | Type | Description |
| -------- | ----------- | ------- | -------------------|
| _id | (um) | INTEGER PRIMARY KEY | Unique ID of the thread. |
| date | (m) | INTEGER | Date of the last modification of the thread. |
| msg_count | (m) | INTEGER | Number of messages in the thread. |
| read | (m) | INTEGER |Value that defines number of unread messages in the thread. Value of zero defines that all messages were seen by the user. |
| contact_id | (m) | TEXT | ID of the contact involved in the thread. Values refer to **contacts**. |
| snippet | (m) | TEXT | First row of the last message in given thread. Text is encoded in UTF8 and has up to 45 characters. |
| last_dir | (m) | INTEGER | Defines direction of the last message in the thread. Values are same as in type field of the sms message. |

#### 8. SMS threads counter table
Name: threads_count

| Field | Scope | Type | Description |
| -------- | ----------- | ------- | -------------------|
| _id | (um) | INTEGER PRIMARY KEY | Unique ID. |
| count | (m) | INTEGER | Fields used to count sms threads. Every new thread increments value stored in count by 1. Removing any of the threads will cause decrementation of the count value by 1. Those operations are performed by database via triggers: **on_thread_insert ** and **on_thread_remove** |

#### 9. SMS template table
Name: sms_template

| Field | Scope | Type | Description |
| -------- | ----------- | ------- | -------------------|
| _id | (um) | INTEGER PRIMARY KEY | Unique ID. |
| text | (m) | TEXT | Text field for storing body of the sms template |
| lastUsageTimestamp | (m) | INTEGER | Timestamp of when the templates was lastly used|


#### 10. Phone settings
Name: settings

| Field | Scope | Type | Description |
| -------- | ----------- | ------- | -------------------|
| _id | (um) | INTEGER PRIMARY KEY | Unique ID. |
| time_format_12 | (m) | BOOL | if true time will be displayed as number with AM or PM suffix. Otherwise it wil be displayed in 24h format.  |
| time_auto | (m) | BOOL | If true time will be automatically updated from GSM network. |
| brightness_auto | (m) | BOOL | If true brightness of the screen will be controlled using light sensor. |
| brightness_level | (m) | BOOL | If auto mode is disabled this value will be used to set brightness of the display. |
| bigger_font | (m) | BOOL | If true bigger font is used in applications. |
| pin_mode | (m) | INTEGER | Mode how user will be requested for PIN entering |
| pin_days | (m) | INTEGER | Number of days after which user will be requested to reenter PIN. |
| pin_days_left | (m) | INTEGER | Number of days until PIN will have to be reentered. |
| pin1_string | (m) | TEXT | Raw string with PIN for sim card 1. |
| pin2_string | (m) | TEXT | Raw string with PIN for sim card 2. |
| active_sim | (m) | INTEGER | Index of the SIM card selected by the user. By default it is first SIM card is selected: value 0 |
| network_operator | (m) | TEXT	 | Raw string with selected network operator. |
| lock_pass_hash | (m) | INTEGER | Hash of the password required for unlocking phone. |
| lock_time| (m) | INTEGER | time of inactivity of the user after which phone will be automatically blocked. Expressed in milliseconds. Default value is 30 000ms |
| language | (m) | INTEGER | Language selected by user. English by default. |

Table describes possible values of **pin_mode** field.

| Name | Value |  Description |
| -------- | ------- | -------------------|
| ALWAYS | 0x01| User wil have to enter PIN after each restart. | 
| DAYS | 0x02| Phone will ask for PIN after given number of days will elapse. |

Table describes possible values of **language** field.

| Name | Value |  Desciption |
| -------- | ------- | -------------------|
| ENGLISH | 0x01| English language. | 
| POLISH| 0x02| Polish language. |
| GERMAN| 0x04| German language. |
| SPANISH| 0x08| Spanish language. |

#### 11. Table with BT paired devices.
Name: bt_devices.

| Field | Scope | Type | Description |
| -------- | ----------- | ------- | -------------------|
| _id | (um) | INTEGER PRIMARY KEY | Unique ID. |
| mac | (m) | TEXT | MAC address of the paired BT device.  |
| flags | (m) | INTEGER | BT device flags. |
| name | (m) | TEXT | Name of the device. |
| link_key | (m) | TEXT | Pairing key. |

#### 12. Table with information about notes.
Name: notes.

| Field | Scope | Type | Description |
| -------- | ----------- | ------- | -------------------|
| _id | (um) | INTEGER PRIMARY KEY | Unique ID. |
| date | (m) | INTEGER | date of note's last modification. |
| snippet | (m) | TEXT | Up to 60 first characters from the note. |
| path | (m) | TEXT | Path to the note's file in the file system. |

#### 13. Table with information about alarms
Name: alarms

| Field | Scope | Type | Description |
| -------- | ----------- | ------- | -------------------|
| _id | (um) | INTEGER PRIMARY KEY | Unique ID. |
| time | (m) | INTEGER | Number of seconds from midnight that needs to elapse to generate alarm event.  |
| snooze | (m) | INTEGER | Number of seconds from alarm that needs to elapse to generate alarm event after snooze.  |
| status | (m) | INTEGER | State of the timer as defined in the table below. |
| path | (m) | TEXT | Path to sound file that should be played when alarm event is generated. |

Table describes possible states of **status** field for alarms.

| Name | Value |  Description |
| -------- | ------- | -------------------|
|ARMED| 0x01| Timer is activated. At specified time in the day timer will play selected music. | 
|DORMANT| 0x02| Timer is deactivated. No event will happen at the time specified in timer. |

#### 14. Table with information about calls
Name: calllog.

| Field | Scope | Type | Description |
| -------- | ----------- | ------- | -------------------|
| _id | (um) | INTEGER PRIMARY KEY | Unique ID. |
| number | (m) | TEXT | date of note's last modification. |
| presentation | (m) | INTEGER | Up to 60 first characters from the note. |
| date | (m) | INTEGER | Call timestamp |
| duration | (m) | INTEGER | Call duration |
| type | (m) | INTEGER | Call type  |
| name | (m) | TEXT | Name associated with the phone number |
| contactId | (m) | TEXT | Unique ID of the record from the **contacts** table from **contacts** database. Stored as TEXT to keep more than one contactIDs in case of multiuser teleconferences |
| isRead | (m) | INTEGER | Boolean value that defines whether message was seen by the user |

**presentation** field - Taken from https://developer.android.com/reference/android/provider/CallLog.Calls.html

| Name | Value | Description |
| ---- | ----- | ----------- |
| PR_UNKNOWN | 0x00 | Presentation unknown |
| PR_ALLOWED | 0x01 | Number is allowed to display for caller id. |
| PR_PAYPHONE | 0x02 | Number is a pay phone. |
| PR_RESTRICTED | 0x03 | Number is blocked by user. |

**type** field - Taken from https://developer.android.com/reference/android/provider/CallLog.Calls.html

| Name | Value | Description |
| ---- | ----- | ----------- |
| CT_NONE | 0x00 | Call type not set |
| CT_INCOMING | 0x01 | Call log type for incoming calls. |
| CT_OUTGOING | 0x02 | Call log type for outgoing calls.|
| CT_MISSED | 0x03 | Call log type for missed calls. |
| CT_VOICEMAIL | 0x04 | Call log type for voicemails. |
| CT_REJECTED | 0x05 | Call log type for calls rejected by direct user action. |
| CT_BLOCKED | 0x06 | Call log type for calls blocked automatically. |
| CT_ANSW_EXT | 0x07 | Call log type for a call which was answered on another device. Used in situations where a call rings on multiple devices simultaneously and it ended up being answered on a device other than the current one. |

#### 14. Table with information about notifications
Name: notifications.

| Field | Scope | Type | Description |
| -------- | ----------- | ------- | -------------------|
| _id | (um) | INTEGER PRIMARY KEY | Unique ID. |
| key | (m) | INTEGER | unique notification key identifier |
| value | (m) | INTEGER | number of notifcations |

**key** field

| Name | Value | Description |
| ---- | ----- | ----------- |
| NotValidKey | 0x00 | Not valid key |
| Calls | 0x01 | Missed calls notifications |
| Sms | 0x02 | Unread incoming sms notifications |

#### 15. Dictionary defining groups
Name: contact_groups

| Field | Scope | Type | Description |
| -------- | ----------- | ------- | -------------------|
| _id | (um) | INTEGER PRIMARY KEY | Unique ID. |
| name | (um) | STGRING | Group name |

##### 15.1 Default groups
By default sepecial gorups are created:

| ID | Name | Description |
|----|------|-------------|
| 1 | Favourites | Defines favourites group. |
| 2 | ICE | Defines ICE group. |
| 3 | Blocked | Defines Blocked group. |
| 4 | Temporary | Defines temporary or deleted contacts. |

#### 16. Joining table for groups and contacts
| Field | Scope | Type | Description |
| -------- | ----------- | ------- | -------------------|
| _id | (um) | INTEGER PRIMARY KEY | Unique ID. |
| group_id | (r) | INTEGER | Id of group. |
| contact_id | (r) | INTEGER | Id of contact. |

#### 17. Events table
Name: events

| ID | Title | Description |
| -------- | ----------- | ------- | -------------------|
| _id | (um) | INTEGER PRIMARY KEY | Unique number assigned to the event |
| title | (m) | TEXT | Event title provided by the user. |
| description | (m) | TEXT | Event description provided by the user. |
| date_from | (m) | INTEGER | Event start time provided by the user. |
| date_till | (m) | INTEGER | Event end time provided by the user. |
| reminder | (m) | INTEGER | Event reminder time provided by the user. |
| repeat | (m) | INTEGER | Event repeat option provided by the user. |
| time_zone | (m) | INTEGER | Event creation time zone imported from settings. Needed to calculate the offset of event while changing time zone |

#### 18. Settins table - newer approach
Name: settings_v2

| ID | Path | Value |
| -------- | ----------- | ------- | -------------------|
| _id | (um) | INTEGER PRIMARY KEY | Unique ID. |
| path | (um) | TEXT | Path of setting. |
| value | (m) | TEXT | Value of setting. |

## Database Triggers

This trigger is responsible for taking action when new thread is created and inserted to threads table. As a result value of the count column with _id equal to 1 in the threads_count table is incremented by 1.
```
CREATE TRIGGER on_thread_insert AFTER INSERT ON threads BEGIN UPDATE threads_count SET count=count+1 WHERE _id=1; END;
```

This trigger is responsible for taking action when thread is removed from threads table. As a result value of the count column with _id equal to 1 in the threads_count table is decremented by 1.
```
CREATE TRIGGER on_thread_remove AFTER DELETE ON threads BEGIN UPDATE threads_count SET count=count-1 WHERE _id=1; END;
```

## SMtask service & DBUS communication

Database related task is created by ```Database_Service_Init()``` function. This function is responsible for all database initialisation & task creation.

SMTask handles used:

* initHandler - used to initialise SQLite, open DB files & initialise tables if necessary
* closeHandler - responsible for closing DB files & shutting SQLite
* sleepHandler - responsible to close DB files
* wakeupHandler - used to openDB files after sleep
* runHandler - main task routine

For communication & control 2 DBUS channels are created:

* ```DB_TASK_CHANNEL_DB_RESPONSES```
* ```DB_TASK_CHANNEL_DB_COMMANDS```

**Preferences (TBD)**

* timezone (+/- n hours from UTC)
* language

Commands available:

```
    DB_SMS_ADD,
    DB_SMS_DEL,
    DB_SMS_FIND_ID,
    DB_SMS_FIND,
    DB_SMS_GET_COUNT,
    DB_SMS_GET_RANGE,
    DB_SMS_GET_ALL,
    DB_CONTACTS_ADD,
    DB_CONTACTS_DEL,
    DB_CONTACTS_FIND_ID,
    DB_CONTACTS_FIND,
    DB_CONTACTS_GET_COUNT,
    DB_CONTACTS_GET_RANGE,
    DB_CONTACTS_GET_ALL,
    DB_GROUPS_GET_GROUPS
```

Each command must be accompanied by ```DBDBusMessage_t``` structure which containes data for each command. Each response is accompanied by similar structure containing returned data.
All structures & data types are described in ```db.h``` file.

##### Database open & close
If client wants to access SMS or Contacts database ```DB_OPEN_SMS_DB``` or ```    DB_OPEN_CONTACTS_DB``` command has to be sent first. When database transaction is finished, database has to be closed in order to flush it's internal buffers & commit all transactions. This can be done by sending ```DB_CLOSE_SMS_DB``` & ```DB_CLOSE_CONTACTS_DB``` commands to database service.

## Database Triggers

This trigger is responsible for taking action when new thread is created and inserted to threads table. As a result value of the count column with _id equal to 1 in the threads_count table is incremented by 1.
```
CREATE TRIGGER on_thread_insert AFTER INSERT ON threads BEGIN UPDATE threads_count SET count=count+1 WHERE _id=1; END;
```

This trigger is responsible for taking action when thread is removedf rom threads table. As a result value of the count column with _id equal to 1 in the threads_count table is decremented by 1.
```
CREATE TRIGGER on_thread_remove AFTER DELETE ON threads BEGIN UPDATE threads_count SET count=count-1 WHERE _id=1; END;
```

## References

Reference | File
--------- | --------
User Stories - Adding a new contact | [g-drive]/pure stories in progress/Copy of Phonebook
Plan for Pure | [g-drive]/Plan for Pure
