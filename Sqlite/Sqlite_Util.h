
#ifndef SQLITE_UTIL
#define SQLITE_UTIL



int Normal_Callback(void *NotUsed,int argc,char **argv,char **azColName)
{
	std::string col_name("NULL");
	std::string col_value("NULL");

	for(int i = 0 ;i<argc ; i++)
	{
		if(azColName[i] != nullptr){
			col_name = azColName[i];
		}
		if(argv[i] != nullptr){
			col_value = argv[i];
		}

		std::cout<<"| "<<col_name<<" =  "<<col_value<<" | ";
	}
	std::cout<<std::endl;

	return 0;
}

class SqlitePack
{
	
	private:
		sqlite3* db;
	public:
		int Open_Database(const std::string );
		int Execute_SqlCmd(const std::string );
};

int SqlitePack::Open_Database(const std::string dbName)
{
	int rc ; 
	rc = sqlite3_open(dbName.c_str(),&db);
	return rc;
}


int SqlitePack::Execute_SqlCmd(const std::string sqlCmd)
{
	char *zErrMsg = nullptr;
	int rc = sqlite3_exec(db,sqlCmd.c_str(),Normal_Callback,0,&zErrMsg);

	if( rc != SQLITE_OK ){
		fprintf(stderr,"SQL EXEC FAIL %s \n",zErrMsg);
		sqlite3_free(zErrMsg);
	}else{
		fprintf(stderr,"SQL EXEC SUCCESSFULLY \n");
	}

	return rc;
}
#endif
