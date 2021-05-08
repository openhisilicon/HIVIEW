#ifndef __db_api_h__
#define __db_api_h__

//https://www.sqlite.org/datatype3.html
//https://www.twblogs.net/a/5e5422bcbd9eee2116837102

#include <stdint.h>
#include <sqlite3.h>

typedef enum _DB_FORM_TYPE_E{
  DB_FORM_FACE = 0,
  DB_FORM_EVENT= 1,
}DB_FORM_TYPE_E;

typedef struct db_face_row_s
{
  uint64_t uuid;
  uint32_t date;
  char *name;
  int  age;
  int  gender;
  char *filepath; // string;
  char *features; // blob_size: sizeof(float)*512;
}db_face_row_t;

typedef struct db_event_row_s
{
  uint64_t uuid;
  uint32_t date;
  int   type;
  int   parm;
  char  *data;     // string;
  char  *filepath; // string;
}db_event_row_t;


typedef struct db_row_s
{
  DB_FORM_TYPE_E type;
  union
  {
    db_event_row_t event;
    db_face_row_t  face;
  };
}db_row_t;

typedef struct db_hdl_s
{
  sqlite3* db;
  char file[256];
}db_hdl_t;

//typedef void db_hdl_t;

typedef int(DB_SELECT_CB)(db_row_t *row, void *_u);

int db_create(char *db_file);
int db_drop(db_hdl_t *_db);

db_hdl_t*
    db_open(char *db_file);
int db_close(db_hdl_t *_db);

int db_instet(db_hdl_t *_db, db_row_t *_row);
int db_delete(db_hdl_t *_db, DB_FORM_TYPE_E form, uint64_t uuid);
int db_update(db_hdl_t *_db, db_row_t *_row);
int db_select(db_hdl_t *_db, DB_FORM_TYPE_E form, uint32_t b, uint32_t e, DB_SELECT_CB *cb, void *_u);



#endif //__db_api_h__
