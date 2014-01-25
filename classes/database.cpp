#include "database.hpp"

using namespace std;
NM_database * NM_database::instance = NULL;
pthread_mutex_t NM_database::mutex;

NM_database::NM_database()
{
    this->db = NULL;
    
    if(!file_exists(::NM_database_filename))
    {
        string query = "BEGIN TRANSACTION; CREATE TABLE `objects` ("
                "`id` INTEGER, "
                "`master_id` INTEGER, "
                "`type` INTEGER, "
                "`name` TEXT, "
                "`info` TEXT, "
                "`date` TEXT "
            ");"
            "CREATE TABLE `net_usage` ("
                "`date` INTEGER," // czas będzie przechowywany jako timestamp aby łatwiej wczytywać dane z pewnego zakresu czasu
                "`program_id` INTEGER,"
                "`user_id` INTEGER,"
                "`device_id` INTEGER,"
                "`remote_id` INTEGER,"
                "`rx_size` REAL," // trzymane na 8-bajtach więc powinno się zmieścić
                "`tx_size` REAL"
            ");"
            "CREATE TABLE `objects_summary` ("
                "`master_id` INTEGER,"
                "`type` INTEGER,"
                "`rx_size` REAL,"
                "`tx_size` REAL"
            ");"
            "CREATE TABLE `objects_compare` ("
                "`master_id` INTEGER,"
                "`type` INTEGER,"
                "`compare_master_id` INTEGER,"
                "`compare_type` INTEGER,"
                "`rx_size` REAL,"
                "`tx_size` REAL"
            ");"
            "CREATE TABLE `optimize_date` ( "
                "`date` TEXT NOT NULL,"
                "`type` TEXT"
            ");"
            "CREATE UNIQUE INDEX objects_summary_id_type ON objects_summary(master_id, type);"
            "CREATE INDEX usage_date ON net_usage(date);"
            "CREATE INDEX objects_id ON objects (id, type);"
            "CREATE INDEX objects_master_id ON objects (master_id, type);"
            "CREATE UNIQUE INDEX object ON objects (master_id, id, type);"
            "CREATE INDEX usage_program_id ON net_usage (program_id);"
            "CREATE INDEX usage_user_id ON net_usage (user_id);"
            "CREATE INDEX usage_device_id ON net_usage (device_id);"
            "CREATE INDEX usage_remote_id ON net_usage (remote_id);"
            "CREATE UNIQUE INDEX usage ON net_usage (date, program_id, user_id, device_id, remote_id);"
            "CREATE UNIQUE INDEX compare_unique ON objects_compare(master_id, type, compare_master_id, compare_type);"
            "CREATE INDEX compare_id ON objects_compare(master_id, type);"
            "CREATE INDEX compare_compare_id ON objects_compare(compare_master_id, compare_type);"
            "CREATE INDEX compare_fetch ON objects_compare(master_id, type, compare_type);"
            "CREATE INDEX compare_fetch_compare ON objects_compare(type, compare_master_id, compare_type);"
            "INSERT INTO optimize_date VALUES(0, 'summary');"
            "INSERT INTO optimize_date VALUES(1, 'compare');"
            "PRAGMA temp_store = 2; END TRANSACTION; "; // save temp file in memory
        try{
            this->query(query);
        }catch(NM_exception e)
        {
            cout << e.get_message() << endl;
            NM_exit(0);
        }
    }
}

void NM_database::open()
{
    pthread_mutex_lock(&NM_database::mutex);
    
    this->error_code = 0;
    this->error_message = "";;
    if(this->db == NULL)
    {
        if(sqlite3_open(::NM_database_filename.c_str(), &this->db) != SQLITE_OK)
        {
            this->error_message = sqlite3_errmsg(this->db);
            this->db = NULL;
            pthread_mutex_unlock(&NM_database::mutex);
            throw NM_exception(NM_ex_code_db_error, this->get_error());
        }
    }
}

void NM_database::close()
{
    if(this->db != NULL)
    {
        if(sqlite3_close(this->db) != SQLITE_OK)
        {
            string error = this->get_error();
            this->db = NULL;
            pthread_mutex_unlock(&NM_database::mutex);
            throw NM_exception(NM_ex_code_db_error, error);
        }
        this->db = NULL;
    }
    pthread_mutex_unlock(&NM_database::mutex);
}

NM_database::~NM_database()
{
    if(this->db != NULL)
        this->close();
    NM_database::instance = NULL;
}

NM_database * NM_database::get_instance()
{
    if(NM_database::instance == NULL)
    {
        NM_database::instance = new NM_database();
        pthread_mutex_init(&NM_database::mutex, NULL);
    }
    return NM_database::instance;
}

NM_2D_string_list NM_database::query(string query)
{   
    this->open();
    NM_2D_string_list results;
    
    if(query.substr(0, 6).compare("SELECT") == 0)
        results = this->select(query);
    else
    {
        this->queries.push_back(query);
        char * errors;
        
        do{
            string query = this->queries[0];
            this->error_code = sqlite3_exec(this->db, query.c_str(), NULL, NULL, &errors);
            if(this->error_code != SQLITE_OK)
                break;
            
            this->queries.erase(this->queries.begin());
        }while(this->queries.size());
        
        
        if(this->error_code != SQLITE_OK)
        {
            this->error_message = sqlite3_errmsg(this->db);
            sqlite3_free(errors);
            ostringstream code;
            code << this->error_code;
            //this->close();
            //throw NM_exception(NM_ex_code_db_error, "Error: (code: " + code.str() + ")'" + this->error_message + "' while executing query: '" + query + "'");
        }
    }
    this->close();
    return results;
}

NM_2D_string_list NM_database::select(string query)
{
    sqlite3_stmt * statement = NULL;
    NM_2D_string_list results;
    this->error_code = sqlite3_prepare_v2(this->db, query.c_str(), -1, &statement, NULL);
    if(this->error_code == SQLITE_OK)
    {
        
        NM_int cols = sqlite3_column_count(statement);
        if(cols > 0)
        {
            while(sqlite3_step(statement) == SQLITE_ROW)
            {
                NM_string_list row;
                for(NM_int col = 0; col < cols; col++)
                {
                    char * buf = NULL;
                    string value = "";
                    // funkcja może zwrócić NULL. Rzutowanie z NULL na string może spowodować błąd w trakcie działania programu
                    buf = (char *)sqlite3_column_text(statement, col);
                    if(buf) value = buf;
                    
                    row.push_back(value);
                }
                results.push_back(row);
            }
        }
        sqlite3_finalize(statement);
        statement = NULL;
    }
    else
        this->error_message = sqlite3_errmsg(this->db);
    return results;
}

bool NM_database::is_error()
{
    return (this->error_code != SQLITE_OK);
}

string NM_database::get_error()
{
    return this->error_message;
}

int NM_database::get_error_code()
{
    return this->error_code;
}
