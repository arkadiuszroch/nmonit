#include "exception.hpp"

using namespace std;

NM_exception::NM_exception(NM_exception_code code, string message)
{
    this->init(code, message);
}

void NM_exception::init(NM_exception_code code, string message)
{
    this->code = code;
    this->message = message;
}

NM_exception_code NM_exception::get_code()
{
    return this->code;
}

string NM_exception::get_message()
{
    return this->message;
}
