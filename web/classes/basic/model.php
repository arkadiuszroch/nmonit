<?php

class NM_basic_model
{
    /**
     *
     * @var SQLite3
     */
    private $connection;
    private $queries;
    
    public function __construct($file = NULL)
    {
        if(!$file)
            $file = DATABASE_FILE;
        
        $this->connection = new SQLite3($file, SQLITE3_OPEN_READONLY);
        if(!$this->connection)
        {
            $this->connection = NULL;
            throw new NM_exception(NM_EX_DB_ERROR, 'Could not connecto to databse file: ' . $file);
        }
        
        $this->queries = array();
    }
    
    public function query($query)
    {
        //$this->queries[] = $query;
        $result = false;
        $start = microtime(true);
        if(preg_match("!^SELECT!", $query))
        {
            $cache = NM_cache::get_instance('NM_model');
            $key = md5($query);
            if($data = $cache->get($key))
                return $data;
            
            $stm = $this->connection->prepare($query);
            
            $result = $stm->execute();
            
            $return = array();
            $i = 1;
            while(($row = $result->fetchArray(SQLITE3_ASSOC)))
            {
                $return[] = $row;
            }
            
            $result->finalize();
            
            $cache->add($key, $return, 120);
        }
        else
        {
            return $this->connection->exec($query);
        }
        return $return;
    }
    
    public function __destruct()
    {
        //vd($this->queries);
        if($this->connection)
        {
            $this->connection->close();
            $this->connection = NULL;
        }
    }
    
    public function is_error()
    {
        return (sqlite_last_error($this->connection));
    }
    
    public function get_error()
    {
        return sqlite_error_string(sqlite_last_error($this->connection));
    }
}
