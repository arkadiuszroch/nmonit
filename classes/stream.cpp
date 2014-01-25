#include "stream.hpp"

using namespace std;

NM_stream::NM_stream()
{
    this->file_name = "";
    this->stream_type = NM_stream_cout;
    this->out = &cout;
    pthread_mutex_init(&this->mutex, NULL);
}

NM_stream::~NM_stream()
{
    if( this->file != NULL && this->file.is_open() )
        this->file.close();

    pthread_mutex_destroy(&this->mutex);
}

void NM_stream::set_stream(NM_stream_type stream_type, string file_name, bool truncate)
{
    this->stream_type = stream_type;

    if( this->stream_type == NM_stream_file )
    {
        if( file_name.length() < 1 )
        {
            throw NM_exception(NM_ex_code_stream_file_empty, "Please specify the file name for stream");
        }
        
        if( this->file.is_open() )
        {
            
            this->file.close();
        }
        this->file.clear();
        if(truncate)
            this->file.open(file_name.c_str(), ios::out | ios::trunc);
        else 
            this->file.open(file_name.c_str(), ios::out | ios::ate);
        
        if( !this->file.is_open() )
        {
            throw NM_exception(NM_ex_code_stream_file_not_open, "Can't open file: " + file_name);
        }

        this->out = &this->file;
        this->file_name = file_name;
    }
    else
    {
        if( this->file != NULL && this->file.is_open() )
        {
            this->file.close();
        }
        this->out = &cout;
    }
}

NM_stream_type NM_stream::get_stream_type()
{
    return this->stream_type;
}

std::string NM_stream::get_file_name()
{
    return this->file_name;
}

ostream & NM_stream::operator <<(bool val)
{
    pthread_mutex_lock(&this->mutex);
    *this->out << val;
    pthread_mutex_unlock(&this->mutex);
    return *this->out;
}

ostream & NM_stream::operator <<(short val)
{
    pthread_mutex_lock(&this->mutex);
    *this->out << val;
    pthread_mutex_unlock(&this->mutex);
    return *this->out;
}

ostream & NM_stream::operator <<(unsigned short val)
{
    pthread_mutex_lock(&this->mutex);
    *this->out << val;
    pthread_mutex_unlock(&this->mutex);
    return *this->out;
}

ostream & NM_stream::operator <<(int val)
{
    pthread_mutex_lock(&this->mutex);
    *this->out << val;
    pthread_mutex_unlock(&this->mutex);
    return *this->out;
}

ostream & NM_stream::operator <<(unsigned int val)
{
    pthread_mutex_lock(&this->mutex);
    *this->out << val;
    pthread_mutex_unlock(&this->mutex);
    return *this->out;
}

ostream & NM_stream::operator <<(long val)
{
    pthread_mutex_lock(&this->mutex);
    *this->out << val;
    pthread_mutex_unlock(&this->mutex);
    return *this->out;
}

ostream & NM_stream::operator <<(unsigned long val)
{
    pthread_mutex_lock(&this->mutex);
    *this->out << val;
    pthread_mutex_unlock(&this->mutex);
    return *this->out;
}

ostream & NM_stream::operator <<(float val)
{
    pthread_mutex_lock(&this->mutex);
    *this->out << val;
    pthread_mutex_unlock(&this->mutex);
    return *this->out;
}

ostream & NM_stream::operator <<(double val)
{
    pthread_mutex_lock(&this->mutex);
    *this->out << val;
    pthread_mutex_unlock(&this->mutex);
    return *this->out;
}

ostream & NM_stream::operator <<(long double val)
{
    pthread_mutex_lock(&this->mutex);
    *this->out << val;
    pthread_mutex_unlock(&this->mutex);
    return *this->out;
}

ostream & NM_stream::operator <<(const void* val)
{
    pthread_mutex_lock(&this->mutex);
    *this->out << val;
    pthread_mutex_unlock(&this->mutex);
    return *this->out;
}

ostream & NM_stream::operator <<(const char * val)
{
    pthread_mutex_lock(&this->mutex);
    *this->out << val;
    pthread_mutex_unlock(&this->mutex);
    return *this->out;
}

ostream & NM_stream::operator <<(const signed char * val)
{
    pthread_mutex_lock(&this->mutex);
    *this->out << val;
    pthread_mutex_unlock(&this->mutex);
    return *this->out;
}

ostream & NM_stream::operator <<(const unsigned char * val)
{
    pthread_mutex_lock(&this->mutex);
    *this->out << val;
    pthread_mutex_unlock(&this->mutex);
    return *this->out;
}

ostream & NM_stream::operator <<(string val)
{
    pthread_mutex_lock(&this->mutex);
    *this->out << val;
    pthread_mutex_unlock(&this->mutex);
    return *this->out;
}

ostream & NM_stream::operator <<(ostream & out)
{
    pthread_mutex_lock(&this->mutex);
    *this->out << out;
    pthread_mutex_unlock(&this->mutex);
    return *this->out;
}