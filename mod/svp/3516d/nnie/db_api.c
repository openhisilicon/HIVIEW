#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>
#include <sqlite3.h>

#include "db_api.h"

/////////////////////////////////////////////////////

#define __BEGIN_CLOCK_CNT() struct timespec __ts = {0, 0};\
                            unsigned int __t1, __t2;\
                            clock_gettime(CLOCK_MONOTONIC, &__ts);\
                            __t1 = __ts.tv_sec*1000 + __ts.tv_nsec/(1000*1000);
                            
#define __END_CLOCK_CNT()  clock_gettime(CLOCK_MONOTONIC, &__ts);\
                            __t2 = __ts.tv_sec*1000 + __ts.tv_nsec/(1000*1000);\
                            printf("cost ms: %d\n", __t2-__t1);



int db_create(char *db_file)
{
  sqlite3* conn = NULL;
  sqlite3_stmt* stmt = NULL;
  int sql_len = 0;
  
  //打开数据库
  int result = sqlite3_open(db_file,&conn);
  if (result != SQLITE_OK)
  {
    goto __error;
  }
 
  /* ----------------------- */
  {
  const char* create_tab_sql = 
      "CREATE TABLE FACE (uuid INTEGER PRIMARY KEY, date INT4, name TEXT DEFAULT '', age INT4"
                            ", gender INT4, filepath TEXT DEFAULT '', features BLOB)";

  sql_len = strlen(create_tab_sql);
  //准备创建数据表，如果创建失败，需要用sqlite3_finalize释放sqlite3_stmt对象，以防止内存泄露。
  if (sqlite3_prepare_v2(conn, create_tab_sql, sql_len, &stmt, NULL) != SQLITE_OK)
  {
    goto __error;
  }
  //通过sqlite3_step命令执行创建表的语句。对于DDL和DML语句而言，sqlite3_step执行正确的返回值
  //只有SQLITE_DONE，对于SELECT查询而言，如果有数据返回SQLITE_ROW，当到达结果集末尾时则返回
  //SQLITE_DONE。
  if (sqlite3_step(stmt) != SQLITE_DONE)
  {
    goto __error;
  }
  
  sqlite3_finalize(stmt);
  }
  /* ----------------------- */
  if(0)
  {
  const char* create_idx_sql = 
    "CREATE INDEX FACE_IDX ON FACE(date)";
    
  sql_len = strlen(create_idx_sql);
  //准备创建数据表，如果创建失败，需要用sqlite3_finalize释放sqlite3_stmt对象，以防止内存泄露。
  if (sqlite3_prepare_v2(conn, create_idx_sql, sql_len, &stmt, NULL) != SQLITE_OK)
  {
    goto __error;
  }
  //通过sqlite3_step命令执行创建表的语句。对于DDL和DML语句而言，sqlite3_step执行正确的返回值
  //只有SQLITE_DONE，对于SELECT查询而言，如果有数据返回SQLITE_ROW，当到达结果集末尾时则返回
  //SQLITE_DONE。
  if (sqlite3_step(stmt) != SQLITE_DONE)
  {
    goto __error;
  }
  
  sqlite3_finalize(stmt);
  }
  /* ----------------------- */
  {
  const char* create_tab_sql = 
      "CREATE TABLE EVENT (uuid INTEGER REFERENCES FACE(uuid) ON DELETE CASCADE, date INT4, type INT4, parm INT4"
                            ", data TEXT, filepath TEXT)";

  sql_len = strlen(create_tab_sql);
  //准备创建数据表，如果创建失败，需要用sqlite3_finalize释放sqlite3_stmt对象，以防止内存泄露。
  if (sqlite3_prepare_v2(conn, create_tab_sql, sql_len, &stmt, NULL) != SQLITE_OK)
  {
    goto __error;
  }
  //通过sqlite3_step命令执行创建表的语句。对于DDL和DML语句而言，sqlite3_step执行正确的返回值
  //只有SQLITE_DONE，对于SELECT查询而言，如果有数据返回SQLITE_ROW，当到达结果集末尾时则返回
  //SQLITE_DONE。
  if (sqlite3_step(stmt) != SQLITE_DONE)
  {
    goto __error;
  }
  
  sqlite3_finalize(stmt);
  }
  /* ----------------------- */
  if(0)
  {
  const char* create_idx_sql = 
    "CREATE INDEX EVENT_IDX ON EVENT(date)";
    
  sql_len = strlen(create_idx_sql);
  //准备创建数据表，如果创建失败，需要用sqlite3_finalize释放sqlite3_stmt对象，以防止内存泄露。
  if (sqlite3_prepare_v2(conn, create_idx_sql, sql_len, &stmt, NULL) != SQLITE_OK)
  {
    goto __error;
  }
  //通过sqlite3_step命令执行创建表的语句。对于DDL和DML语句而言，sqlite3_step执行正确的返回值
  //只有SQLITE_DONE，对于SELECT查询而言，如果有数据返回SQLITE_ROW，当到达结果集末尾时则返回
  //SQLITE_DONE。
  if (sqlite3_step(stmt) != SQLITE_DONE)
  {
    goto __error;
  }
  
  sqlite3_finalize(stmt);
  }

  sqlite3_close(conn);
  return 0;

__error:
    if(stmt)
      sqlite3_finalize(stmt);
    if(conn)
      sqlite3_close(conn);
      
    return -1;
}

int db_drop(db_hdl_t *_db)
{
  sqlite3* db = (sqlite3*)_db->db;
  sqlite3_stmt* stmt = NULL;

  /*------------------------*/
  if(0)
  { 
  const char* drop_idx_sql = "DROP INDEX EVENT_IDX";

  if (sqlite3_prepare_v2(db,drop_idx_sql,strlen(drop_idx_sql),&stmt,NULL) != SQLITE_OK) 
  {
    goto __error;
  }
  if (sqlite3_step(stmt) == SQLITE_DONE)
  {
    ;
  }
  sqlite3_finalize(stmt);
  }
  /*------------------------*/
  {
  const char* drop_tab_sql = "DROP TABLE EVENT";

  if (sqlite3_prepare_v2(db,drop_tab_sql,strlen(drop_tab_sql),&stmt,NULL) != SQLITE_OK) 
  {
    goto __error;
  }
  if (sqlite3_step(stmt) == SQLITE_DONE)
  {
    ;
  }
  sqlite3_finalize(stmt);
  }
  /*------------------------*/
  if(0)
  { 
  const char* drop_idx_sql = "DROP INDEX FACE_IDX";

  if (sqlite3_prepare_v2(db,drop_idx_sql,strlen(drop_idx_sql),&stmt,NULL) != SQLITE_OK) 
  {
    goto __error;
  }
  if (sqlite3_step(stmt) == SQLITE_DONE)
  {
    ;
  }
  sqlite3_finalize(stmt);
  }
  /*------------------------*/
  {
  const char* drop_tab_sql = "DROP TABLE FACE";

  if (sqlite3_prepare_v2(db,drop_tab_sql,strlen(drop_tab_sql),&stmt,NULL) != SQLITE_OK) 
  {
    goto __error;
  }
  if (sqlite3_step(stmt) == SQLITE_DONE)
  {
    ;
  }
  sqlite3_finalize(stmt);
  }
  /*------------------------*/
  {
  const char* drop_tab_sql = "DROP TABLE PICTURE";

  if (sqlite3_prepare_v2(db,drop_tab_sql,strlen(drop_tab_sql),&stmt,NULL) != SQLITE_OK) 
  {
    goto __error;
  }
  if (sqlite3_step(stmt) == SQLITE_DONE)
  {
    ;
  }
  sqlite3_finalize(stmt);
  }
  /*------------------------*/
  return 0;
  
__error:
  if(stmt)
    sqlite3_finalize(stmt);
  return -1;
}

/* 
** Callback from the integrity check.  If the result is anything other
** than "ok" it means the integrity check has failed. Print the error message
** or print OK if the string "ok" is seen.
*/
static int db_check_callback(void *data, int argc, char **argv, char **notUsed2)
{
	int *flag = (int *)data;

	/* 数据库不完整，设置标记为-1 */
	if (strcmp(argv[0], "ok"))
	{
		*flag = -1;
		printf("%s\n",  argv[0]);	/* 打印错误信息 */
	}
	else
	{
		*flag = 0;
	}
	return 0;
}

int check_active_db_integrity(struct sqlite3 *db)
{
	int rc = -1;
	int db_ok = -1;		/* 数据库结构是否完整的标记 */
	char *err_msg = NULL;
	
	/* 其中，db_ok会传递给回调函数db_check_callback的第一个参数。*/
	rc = sqlite3_exec(db, "pragma integrity_check", db_check_callback, &db_ok, &err_msg);
	if(rc != SQLITE_OK)
	{
		printf(" Integrity check returns %d\n", rc);
		printf(" %s\n", err_msg);
		sqlite3_free(err_msg);
		return -1;
	}
	/* 数据库文件结构完整，返回OK */
	if (0 == db_ok)
	{
		return 0;
	}

	return -1;
}

void  db_disconnect (sqlite3 *db)
{
	int ret = -1;
	
	if(db != NULL)
	{
		ret = sqlite3_close(db);
		if (ret != SQLITE_OK)
		{
			printf("Close database failure, waiting for 500ms to try again\n");
			/* 等待一段时间再关闭,防止数据库正忙碌 */
			usleep(500*1000);
			(void)sqlite3_close(db);
		}
	}
}

int check_inactive_db_integrity(const char *db_name)
{
	int ret = -1;
	sqlite3 *db = NULL;
		
	// 检查传入值是否正确
	if(NULL == db_name)
	{
		printf(" the dbname is NULL.\n");
		return -1;
	}
	
	// 打开指定的数据库文件,如果不存在将创建一个同名的数据库文件
	ret = sqlite3_open(db_name, &db);
	if (ret != SQLITE_OK)
	{
		printf(" sqlite3_open failed.\n");
		return -1;
	}
	// 检查已打开db完整性
	ret = check_active_db_integrity(db);
	(void)db_disconnect(db);

	return ret;
}


db_hdl_t* db_open(char *db_file)
{
  db_hdl_t *new_db_hdl = NULL;
  sqlite3* conn = NULL;
  sqlite3_stmt* stmt = NULL;

  // 检查数据库文件是否存在, 不存在时尝试从最后一次的备份文件进行恢复;
  if(access(db_file, R_OK|W_OK) < 0)
  {
    char bak_file[256] = {0};
    char _db_file[256] = {0};
    strncpy(_db_file, db_file, sizeof(_db_file)-1);
    snprintf(bak_file, sizeof(bak_file), "%s/face.bak", dirname(_db_file));
    if(access(bak_file, R_OK|W_OK) < 0)
    {
      if(db_create(db_file) < 0)
      {
        printf("err: access db_file:%s && bak_file:%s\n", db_file, bak_file);
        return NULL;
      }
      else
      {
        printf("warn: create db_file:%s!\n", db_file);
      }
    }
    else
    {
      char buf[256] = {0};
      snprintf(buf, sizeof(buf), "mv %s %s", bak_file, db_file);
      (void)system(buf);
      printf("ok: mv bak_file to db_file\n", db_file);
    }
  }
  else
  {
    printf("ok: db_file:%s is exist!\n", db_file);
  }
  
  //检查数据库文件完整性
  if (check_inactive_db_integrity(db_file) < 0)
  {
  	printf("err: integrity db: %s\n", db_file);
  	return NULL;
  }
  else
  {
      printf("ok: integrity db: %s\n", db_file);
  }
  
  //打开数据库
  int result = sqlite3_open_v2(db_file,&conn, SQLITE_OPEN_READWRITE /*| SQLITE_OPEN_FULLMUTEX*/, NULL);
  if (result != SQLITE_OK)
  {
    if(conn)
      sqlite3_close(conn);
    printf("err: db_open ===> return NULL.\n");
    return NULL;
  }

  printf("ok: db_open ===> conn: %p, thread_safe_flags :%d\n", conn, sqlite3_threadsafe());

  /*-----------------------------------*/
  {
    const char *parm = "PRAGMA journal_mode = WAL";
    if(sqlite3_exec(conn, parm, 0, 0, 0) != SQLITE_OK)
    {
      goto __error;
    }
    printf("ok: db_open ===> parm:[%s]\n", parm);
  }
  /*-----------------------------------*/
  {
    const char *parm = "PRAGMA foreign_keys = ON";
    
    if (sqlite3_prepare_v2(conn, parm, strlen(parm), &stmt, NULL) != SQLITE_OK)
    {
      goto __error;
    }
    if (sqlite3_step(stmt) != SQLITE_DONE) 
    {
      goto __error;
    }
    sqlite3_finalize(stmt);
    printf("ok: db_open ===> parm:[%s]\n", parm);
  }
  /*-----------------------------------*/

  new_db_hdl = (db_hdl_t *) malloc(sizeof(db_hdl_t));
  if (NULL == new_db_hdl)
  {
  	printf("err :malloc new_db_hdl failed, db_file :%s\n", db_file);
  	goto __error;
  }

  new_db_hdl->db = conn;
  strncpy(new_db_hdl->file, db_file, sizeof(new_db_hdl->file)-1);
  new_db_hdl->file[sizeof(new_db_hdl->file)-1] = '\0';
  
  return new_db_hdl;
  //return (db_hdl_t*)conn;
  
__error:
  if(stmt)
    sqlite3_finalize(stmt);
  if(conn)
    sqlite3_close(conn);

  printf("err: db_open ===> db_file:[%s]\n", db_file);
  return NULL;
}

int db_close(db_hdl_t *_db)
{
  if(!_db)
    return -1;
    
  sqlite3* db = (sqlite3*)_db->db;
  if(db)
  {
    printf("_db->file:%s\n", _db->file);
    sqlite3_close(db);
  }
  free(_db);
  return 0;
}


int db_instet(db_hdl_t *_db, db_row_t *_row)
{
  sqlite3* db = (sqlite3*)_db->db;
  sqlite3_stmt* stmt = NULL;
  
  const char* insert_face = "INSERT INTO FACE VALUES(%llu,%u,'%s',%u,%u,'%s',:features)";
  const char* insert_event = "INSERT INTO EVENT VALUES(%llu,%u,%u,%u,'%s','%s')";

  
  char sql[1024];
  
  if(_row->type == DB_FORM_FACE)
  {
      db_face_row_t *row = &_row->face;
      sprintf(sql, insert_face, row->uuid, row->date, row->name, row->age
              , row->gender, row->filepath);
  }
  else if(_row->type == DB_FORM_EVENT)
  {
      db_event_row_t *row = &_row->event;
      sprintf(sql, insert_event, row->uuid, row->date, row->type, row->parm, row->data
              , row->filepath);
  }
  else
  {
      return -1;
  }
  
  printf("sql:[%s]\n", sql);
  
  if (sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL) != SQLITE_OK)
  {
    goto __error;
  }
  if(_row->type == DB_FORM_FACE)
  {
    db_face_row_t *row = &_row->face;
    int index = sqlite3_bind_parameter_index(stmt, ":features");
    if(sqlite3_bind_blob(stmt, index, row->features, sizeof(float)*512, NULL) != SQLITE_OK)
    {
      goto __error;
    }
  }
  if (sqlite3_step(stmt) != SQLITE_DONE) 
  {
    goto __error;
  }
  
  sqlite3_finalize(stmt);
  
  return 0;

__error:
  if(stmt)
    sqlite3_finalize(stmt);
  
  printf("err: db_instet ===> sql=[%s]\n", sql);
  return -1;
}


int db_delete(db_hdl_t *_db, DB_FORM_TYPE_E form, uint64_t uuid)
{
  sqlite3* db = (sqlite3*)_db->db;
  sqlite3_stmt* stmt = NULL;
  const char* delete_face = "DELETE FROM FACE WHERE uuid=%llu";
  const char* delete_event = "DELETE FROM EVENT WHERE uuid=%llu";

  char sql[1024];


  if(form == DB_FORM_FACE)
  {
    sprintf(sql, delete_face, uuid);
  }
  else if(form == DB_FORM_EVENT)
  {
    sprintf(sql, delete_event, uuid);
  }
  else
  {
      return -1;
  }  

  if (sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL) != SQLITE_OK)
  {
    goto __error;
  }
  if (sqlite3_step(stmt) != SQLITE_DONE) 
  {
    goto __error;
  }
  
  sqlite3_finalize(stmt);
  
  return 0;
__error:
  if(stmt)
    sqlite3_finalize(stmt);

  printf("err: db_delete ===> sql=[%s]\n", sql);
  return -1;
}

int db_update(db_hdl_t *_db, db_row_t *_row)
{
  sqlite3* db = (sqlite3*)_db->db;
  sqlite3_stmt* stmt = NULL;

  const char* update_face = "UPDATE FACE SET date=%u, name='%s', age=%u"
                          ", gender=%u, filepath='%s', features=:features WHERE uuid=%llu";
  const char* update_event = "UPDATE EVENT SET date=%u, type=%u, parm=%u, data='%s'"
                          ", filepath='%s' WHERE uuid=%llu";

  char sql[1024];

  if(_row->type == DB_FORM_FACE)
  {
      db_face_row_t *row = &_row->face;
      sprintf(sql, update_face, row->date, row->name, row->age
              , row->gender, row->filepath, row->uuid);
  
  }
  else if(_row->type == DB_FORM_EVENT)
  {
      db_event_row_t *row = &_row->event;
      sprintf(sql, update_event, row->date, row->type, row->parm, row->data
              , row->filepath, row->uuid);
  }
  else
  {
      return -1;
  }

  printf("db_update >>>>>>>>>>>>>> sql :%s\n", sql);
  
  if (sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL) != SQLITE_OK)
  {
    goto __error;
  }
  if(_row->type == DB_FORM_FACE)
  {
    db_face_row_t *row = &_row->face;
    int index = sqlite3_bind_parameter_index(stmt, ":features");
    if(sqlite3_bind_blob(stmt, index, row->features, sizeof(float)*512, NULL) != SQLITE_OK)
    {
      goto __error;
    }
  }
  if (sqlite3_step(stmt) != SQLITE_DONE) 
  {
    goto __error;
  }
  
  sqlite3_finalize(stmt);

  return 0;
  
__error:
  if(stmt)
    sqlite3_finalize(stmt);

  printf("err: db_update ===> sql=[%s]\n", sql);
  return -1;
}



int db_select(db_hdl_t *_db, DB_FORM_TYPE_E form, uint32_t b, uint32_t e, DB_SELECT_CB *cb, void *_u)
{
  sqlite3* db = (sqlite3*)_db->db;
  sqlite3_stmt* stmt = NULL;
  //执行SELECT语句查询数据。
  const char* select_face = "SELECT * FROM FACE WHERE (date<=%u)AND(date>=%u) ORDER BY date";
  const char* select_event = "SELECT * FROM EVENT WHERE (date<=%u)AND(date>=%u) ORDER BY date";
  
  char sql[1024] = {0};

  if(form == DB_FORM_FACE)
  {
    sprintf(sql, select_face, e, b);
  }
  else if(form == DB_FORM_EVENT)
  {
    sprintf(sql, select_event, e, b);
  }
  else
  {
      return -1;
  }

  printf("db_select >>>>>>>>>>>>>> sql :%s\n", sql);
  
  if (sqlite3_prepare_v2(db,sql,strlen(sql),&stmt,NULL) != SQLITE_OK)
  {
      printf("err >>> [%s]\n", sql);
      if (stmt)
          sqlite3_finalize(stmt);

      return -1;
  }
  printf("ok >>> [%s]\n", sql);
  
  int fieldCount = sqlite3_column_count(stmt);
  if(form == DB_FORM_FACE)
  {
      do {
          int r = sqlite3_step(stmt);
          if (r == SQLITE_ROW) {
              db_row_t _row;
              db_face_row_t *row = &_row.face;
              _row.type = form;
              
              row->uuid     = sqlite3_column_int64(stmt,0);
              row->date     = sqlite3_column_int(stmt,1);
              row->name     = sqlite3_column_text(stmt,2);
              row->age      = sqlite3_column_int(stmt,3);
              row->gender   = sqlite3_column_int(stmt,4);
              row->filepath = sqlite3_column_text(stmt,5);
              row->features = sqlite3_column_blob(stmt,6); 
              //int len = sqlite3_column_bytes(stmt,6);
              if(cb)
              {
                if(cb(&_row, _u))
                {
                    /* user cancle */
                    break;
                }
              }
          } else if (r == SQLITE_DONE) {
              printf("Select Finished.\n");
              break;
          } else {
              printf("Failed to SELECT.\n");
              sqlite3_finalize(stmt);
              return -1;
          }
      } while (1);
  }
  else if(form == DB_FORM_EVENT)
  {
      do {
          int r = sqlite3_step(stmt);
          if (r == SQLITE_ROW) {
              db_row_t _row;
              db_event_row_t *row = &_row.event;
              _row.type = form;

              row->uuid     = sqlite3_column_int64(stmt,0);
              row->date     = sqlite3_column_int(stmt,1);
              row->type     = sqlite3_column_int(stmt,2);
              row->parm     = sqlite3_column_int(stmt,3);
              row->data     = sqlite3_column_text(stmt,4);
              row->filepath = sqlite3_column_text(stmt,5);
              
              if(cb)
              {
                if(cb(&_row, _u))
                {
                    /* user cancle */
                    break;
                }
              }
          } else if (r == SQLITE_DONE) {
              printf("Select Finished.\n");
              break;
          } else {
              printf("Failed to SELECT.\n");
              sqlite3_finalize(stmt);
              return -1;
          }
      } while (1);
  }
  sqlite3_finalize(stmt);
  
  return 0;
}

static int test_cb(db_row_t *row, void *_u)
{
  printf("row->face[uuid:%llu, filepath:%s, features(0x%02x%02x%02x%02x)]\n"
        , row->face.uuid, row->face.filepath
        , row->face.features[0], row->face.features[1], row->face.features[2], row->face.features[3]);
  return 0;
}

int db_test(char *db_file)
{
    int i = 0, ret = 0;
    db_hdl_t* hdl = db_open(db_file);
    printf("create hdl:%p, db_file:%s!\n", hdl, db_file);
    
    while(i++ < 1000)
    {
      db_row_t row;
      row.type = DB_FORM_FACE;
      
      char name[256] = {0};
      char filepath[256] = {0};
      char features[2048] = {0};
      sprintf(filepath, "/tmp/%08d.jpeg", i);
      features[0] = i<<24; features[1] = i<<16; features[2] = i<<8; features[3] = i<<0;
      
      row.face.uuid = i;
      row.face.date = i;
      row.face.name = name;
      row.face.age  = i;
      row.face.gender = 0;
      row.face.filepath = filepath; // string;
      row.face.features = features; // blob_size: sizeof(float)*512;
      
      ret = db_instet(hdl, &row);
    }
    printf("db_instet i:%d!\n", i);
    sleep(3);
    
    ret = db_select(hdl, DB_FORM_FACE, 0, 0xffffffff, test_cb, NULL);
    printf("db_select ret:%d!\n", ret);
    
    ret = db_close(hdl);
    printf("db_close ret:%d!\n", ret);
}