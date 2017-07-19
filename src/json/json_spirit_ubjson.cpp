// Copyright (c) 2014-2017 Coin Sciences Ltd
// MultiChain code distributed under the GPLv3 license, see COPYING file.

#include "json/json_spirit_ubjson.h"

#define UBJ_UNDEFINED          0
#define UBJ_NULLTYPE           1
#define UBJ_NOOP               2
#define UBJ_BOOL_TRUE          3
#define UBJ_BOOL_FALSE         4
#define UBJ_CHAR               5
#define UBJ_STRING             6
#define UBJ_HIGH_PRECISION     7
#define UBJ_INT8               8
#define UBJ_UINT8              9
#define UBJ_INT16             10
#define UBJ_INT32             11
#define UBJ_INT64             12
#define UBJ_FLOAT32           13
#define UBJ_FLOAT64           14
#define UBJ_ARRAY             15
#define UBJ_OBJECT            16
#define UBJ_STRONG_TYPE       17
#define UBJ_COUNT             18


char UBJ_TYPE[19] ={0x00,'Z','N','T','F','C','S','H','i','U','I','l','L','d','D','[','{','$','#'};
int  UBJ_SIZE[19] ={   0,  0,  0,  0,  0,  0, -1, -1,  1,  1,  2,  4,  8,  4,  8, -2, -3, -4, -5};
char UBJ_ISINT[19]={   0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  0,  0,  0,  0,  0,  0};
int UBJ_INTERNAL_TYPE[256]=
{
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, UBJ_COUNT, UBJ_STRONG_TYPE, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, UBJ_CHAR, UBJ_FLOAT64, -1, UBJ_BOOL_FALSE, -1, UBJ_HIGH_PRECISION, UBJ_INT16, -1, -1, UBJ_INT64, -1, UBJ_NOOP, -1,
    -1, -1, -1, UBJ_STRING, UBJ_BOOL_TRUE, UBJ_UINT8, -1, -1, -1, -1, UBJ_NULLTYPE, UBJ_ARRAY, -1, -1, -1, -1,
    -1, -1, -1, -1, UBJ_FLOAT32, -1, -1, -1, -1, UBJ_INT8, -1, -1, UBJ_INT32, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, UBJ_OBJECT, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};

int ubjson_best_type(Value json_value,int last_type,int *not_uint8,int64_t *usize,int64_t *ssize)
{
    char type;
    string string_value;
    int64_t int64_value;
    uint64_t uint64_value;
    int size;

    type=UBJ_UNDEFINED;
    switch(json_value.type())
    {
        case null_type:
            type=UBJ_NULLTYPE;
            break;
        case bool_type:
            type=UBJ_BOOL_FALSE;
            if(json_value.get_bool())
            {
                type=UBJ_BOOL_TRUE;
            }
            break;
        case int_type:
            int64_value=json_value.get_int64();
            uint64_value=*(uint64_t*)&int64_value;
            if(uint64_value < 0x80)
            {
                type=UBJ_INT8;
                if(int64_value < 0)
                {
                    if(not_uint8)
                    {
                        *not_uint8=1;
                    }
                }
            }
            else
            {
                if((uint64_value < 0x100) && (int64_value >=0 ) )
                {
                    type=UBJ_UINT8;                    
                }          
                else
                {
                    if(not_uint8)
                    {
                        *not_uint8=1;
                    }                    
                    if( uint64_value < 0x8000 )
                    {
                        type=UBJ_INT16;
                    }
                    else
                    {
                        if( uint64_value < 0x80000000 )
                        {
                            type=UBJ_INT32;
                        }
                        else
                        {
                            type=UBJ_INT64;                            
                        }
                    }
                }
            }
            break;
        case real_type:
            type=UBJ_FLOAT64;
            break;
        case str_type:
            string_value=json_value.get_str();
            if( (string_value.size() == 1) && (*(string_value.c_str()) >= 0) )
            {
                type=UBJ_CHAR;
            }
            else
            {
                type=UBJ_STRING;
            }
            break;
        case array_type:
            type=UBJ_ARRAY;
            break;
        case obj_type:
            type=UBJ_OBJECT;
            break;            
    }

    size=UBJ_SIZE[type];
    if(usize)
    {
        if(size>=0)
        {
            *usize+=size;
            if(ssize)
            {
                *ssize+=size;
                if(type == UBJ_UINT8)
                {
                    *ssize+=1;
                }
            }
        }
    }
    
    if(last_type != UBJ_UNDEFINED)
    {
        if(type != last_type)
        {
            if(UBJ_ISINT[last_type] & UBJ_ISINT[type]) 
            {
                if(type < last_type)
                {
                    type=last_type;
                }
            }
            else
            {
                type=-1;
            }
        }
    }
    
    return type;
}

int ubjson_int64_write(int64_t int64_value,int known_type,mc_Script *lpScript)
{
    return MC_ERR_NOERROR;
}

int ubjson_write_internal(Value json_value,int known_type,mc_Script *lpScript)
{
    char type;
    int ubj_type,last_type;
    int optimized;
    double double_value;
    string string_value;
    Value value;
    Array array_value;
    Object obj_value;
    int not_uint8;
    int64_t usize,ssize;
    unsigned int i;
    
    ubj_type=known_type;
    if(ubj_type == UBJ_UNDEFINED)
    {
        ubj_type=ubjson_best_type(json_value,0,NULL,NULL,NULL);
        type=UBJ_TYPE[ubj_type];
        lpScript->SetData((unsigned char*)&type,1);
    }
    if(UBJ_ISINT[ubj_type])
    {
        ubjson_int64_write(json_value.get_int64(),ubj_type,lpScript);        
    }
    else
    {
        switch(ubj_type)
        {
            case UBJ_NULLTYPE:
            case UBJ_BOOL_FALSE:
            case UBJ_BOOL_TRUE:
                break;
            case UBJ_FLOAT64:
                double_value=json_value.get_real();
                lpScript->SetData((unsigned char*)&double_value,sizeof(double));
                break;
            case UBJ_CHAR:
                lpScript->SetData((unsigned char*)string_value.c_str(),1);
                break;
            case UBJ_STRING:
                ubjson_int64_write((int64_t)string_value.size(),UBJ_UNDEFINED,lpScript);
                lpScript->SetData((unsigned char*)string_value.c_str(),string_value.size());
                break;
            case UBJ_ARRAY:
                optimized=1;
                last_type=UBJ_UNDEFINED;
                not_uint8=0;
                usize=0;
                ssize=0;
                i=0;
                array_value=json_value.get_array();
                while(i<array_value.size())
                {
                    ubj_type=ubjson_best_type(array_value[i],last_type,&not_uint8,&usize,&ssize);
                    if(ubj_type > 0)
                    {
                        last_type=ubj_type;
                        i++;
                    }
                    else
                    {
                        optimized=0;
                        i=array_value.size();
                    }
                }
                if(optimized)
                {
                    if(last_type == UBJ_UINT8)
                    {
                        if(not_uint8)
                        {
                            last_type=UBJ_INT16;
                        }
                        else
                        {
                            ssize=usize;                            
                        }
                    }
                    value=array_value.size();
                    ubj_type=ubjson_best_type(value,0,NULL,NULL,NULL);
                    if(usize+(int64_t)array_value.size() <= ssize+UBJ_SIZE[ubj_type]+2)
                    {
                        optimized=0;
                    }
                }
                i=0;
                if(optimized)
                {
                    type='$';
                    lpScript->SetData((unsigned char*)&type,1);
                    type=UBJ_TYPE[last_type];
                    lpScript->SetData((unsigned char*)&type,1);
                    type='#';
                    ubjson_int64_write((int64_t)array_value.size(),UBJ_UNDEFINED,lpScript);
                }
                else
                {
                    last_type=UBJ_UNDEFINED;
                }
                while(i<array_value.size())
                {
                    if(optimized && UBJ_ISINT[last_type])
                    {
                        ubjson_int64_write(array_value[i].get_int64(),last_type,lpScript);                                
                    }
                    else
                    {
                        ubjson_write_internal(array_value[i],last_type,lpScript);
                    }
                }                
                if(!optimized)
                {
                    type=']';
                    lpScript->SetData((unsigned char*)&type,1);                    
                }
                break;
            case UBJ_OBJECT:
                optimized=1;
                last_type=UBJ_UNDEFINED;
                not_uint8=0;
                usize=0;
                ssize=0;
                i=0;
                obj_value=json_value.get_obj();
                while(i<obj_value.size())
                {
                    ubj_type=ubjson_best_type(obj_value[i].value_,last_type,&not_uint8,&usize,&ssize);
                    if(ubj_type > 0)
                    {
                        last_type=ubj_type;
                        i++;
                    }
                    else
                    {
                        optimized=0;
                        i=obj_value.size();
                    }
                }
                if(optimized)
                {
                    if(last_type == UBJ_UINT8)
                    {
                        if(not_uint8)
                        {
                            last_type=UBJ_INT16;
                        }
                        else
                        {
                            ssize=usize;                            
                        }
                    }
                    value=obj_value.size();
                    ubj_type=ubjson_best_type(value,0,NULL,NULL,NULL);
                    if(usize+(int64_t)obj_value.size() <= ssize+UBJ_SIZE[ubj_type]+2)
                    {
                        optimized=0;
                    }
                }
                i=0;
                if(optimized)
                {
                    type='$';
                    lpScript->SetData((unsigned char*)&type,1);
                    type=UBJ_TYPE[last_type];
                    lpScript->SetData((unsigned char*)&type,1);
                    type='#';
                    ubjson_int64_write((int64_t)obj_value.size(),UBJ_UNDEFINED,lpScript);
                }
                else
                {
                    last_type=UBJ_UNDEFINED;
                }
                while(i<obj_value.size())
                {
                    ubjson_write_internal(obj_value[i].name_,UBJ_STRING,lpScript);
                    if(optimized && UBJ_ISINT[last_type])
                    {
                        ubjson_int64_write(obj_value[i].value_.get_int64(),last_type,lpScript);                                
                    }
                    else
                    {
                        ubjson_write_internal(obj_value[i].value_,last_type,lpScript);
                    }
                }                
                if(!optimized)
                {
                    type=']';
                    lpScript->SetData((unsigned char*)&type,1);                    
                }
                break;            
            default:
                return MC_ERR_NOT_SUPPORTED;
        }
    }            
            
    return MC_ERR_NOERROR;
}

int ubjson_write(Value json_value,mc_Script *lpScript)
{
    return ubjson_write_internal(json_value,0,lpScript);
}

int64_t ubjson_int64_read(unsigned char *ptrStart,unsigned char *ptrEnd,int known_type,int *shift,int *err)
{
    return 0;
}



Value ubjson_read_internal(mc_Script *lpScript,int offset,int known_type,int *shift,int *err)
{
    Value result;
    
    int ubj_type;
    unsigned char *ptr;
    const unsigned char *ptrStart;
    unsigned char *ptrEnd;
    size_t bytes;
    int size,sh,i;
    Array array_value;
    Object obj_value;
    Value string_value;
    
    
    result=Value::null;
    *err=MC_ERR_NOERROR;
    
    if(lpScript->m_CurrentElement<0)
    {
        return MC_ERR_INVALID_PARAMETER_VALUE;
    }
    
    ptrStart=lpScript->GetData(lpScript->GetElement(),&bytes);
    ptr=(unsigned char *)ptrStart;
    if(ptr == NULL)
    {
        *err=MC_ERR_INVALID_PARAMETER_VALUE;
        goto exitlbl;
    }
    
    ptrEnd=ptr+bytes;
    ptr+=offset;
    
    if(ptr >= ptrEnd)
    {
        *err=MC_ERR_ERROR_IN_SCRIPT;
        goto exitlbl;
    }

    ubj_type=known_type;
    
    if(ubj_type == UBJ_UNDEFINED)
    {
        ubj_type=UBJ_INTERNAL_TYPE[*ptr];
        if(ubj_type < 0)
        {
            *err=MC_ERR_ERROR_IN_SCRIPT;
            goto exitlbl;        
        }
        ptr++;
    }
    
    if(UBJ_ISINT[ubj_type])
    {
        result=ubjson_int64_read(ptr,ptrEnd,ubj_type,&sh,err);
        ptr+=sh;
    }
    else
    {
        switch(ubj_type)
        {
            case UBJ_NULLTYPE:
                result=Value::null;
                break;
            case UBJ_BOOL_FALSE:
                result=false;
                break;
            case UBJ_BOOL_TRUE:
                result=true;
                break;
            case UBJ_FLOAT32:
                if(ptr+UBJ_SIZE[ubj_type] >= ptrEnd)
                {
                    *err=MC_ERR_ERROR_IN_SCRIPT;
                    goto exitlbl;                            
                }                
                result=(double)(*(float*)ptr);
                ptr+=UBJ_SIZE[ubj_type];
                break;
            case UBJ_FLOAT64:
                if(ptr+UBJ_SIZE[ubj_type] >= ptrEnd)
                {
                    *err=MC_ERR_ERROR_IN_SCRIPT;
                    goto exitlbl;                            
                }                
                result=*(double*)ptr;
                ptr+=UBJ_SIZE[ubj_type];
                break;
            case UBJ_CHAR:
                if(ptr+UBJ_SIZE[ubj_type] >= ptrEnd)
                {
                    *err=MC_ERR_ERROR_IN_SCRIPT;
                    goto exitlbl;                            
                }                
                result= string((char*)ptr,1);
                ptr+=UBJ_SIZE[ubj_type];
                break;
            case UBJ_STRING:
                size=(int)ubjson_int64_read(ptr,ptrEnd,UBJ_UNDEFINED,&sh,err);
                if(err)
                {
                    goto exitlbl;
                }
                ptr+=sh;
                if(ptr+size >= ptrEnd)
                {
                    *err=MC_ERR_ERROR_IN_SCRIPT;
                    goto exitlbl;                            
                }                
                result= string((char*)ptr,size);
                ptr+=size;                
                break;
            case UBJ_ARRAY:
                
                ubj_type=UBJ_UNDEFINED;
                if(ptr >= ptrEnd)
                {
                    *err=MC_ERR_ERROR_IN_SCRIPT;
                    goto exitlbl;                            
                }  
                size=-1;
                if(*ptr == UBJ_TYPE[UBJ_STRONG_TYPE])
                {
                    ptr++;
                    if(ptr >= ptrEnd)
                    {
                        *err=MC_ERR_ERROR_IN_SCRIPT;
                        goto exitlbl;                            
                    }  
                    ubj_type=UBJ_INTERNAL_TYPE[*ptr];
                    if(ubj_type < 0)
                    {
                        *err=MC_ERR_ERROR_IN_SCRIPT;
                        goto exitlbl;        
                    }
                    if(ptr >= ptrEnd)
                    {
                        if(*ptr != UBJ_TYPE[UBJ_COUNT])
                        {
                            *err=MC_ERR_ERROR_IN_SCRIPT;
                            goto exitlbl;                                    
                        }
                    }
                }
                if(*ptr == UBJ_TYPE[UBJ_COUNT])
                {
                    ptr++;
                    size=(int)ubjson_int64_read(ptr,ptrEnd,UBJ_UNDEFINED,&sh,err);
                    if(err)
                    {
                        goto exitlbl;
                    }                    
                    ptr+=sh;
                }
                
                if(size >= 0)
                {
                    i=0;
                    while(i<size)
                    {
                        array_value.push_back(ubjson_read_internal(lpScript,ptr-ptrStart,ubj_type,&sh,err));
                        if(err)
                        {
                            goto exitlbl;
                        }                    
                        ptr+=sh;
                        i++;
                    }
                }
                else
                {
                    if(ptr >= ptrEnd)
                    {
                        *err=MC_ERR_ERROR_IN_SCRIPT;
                        goto exitlbl;                            
                    }  
                    while(*ptr != ']')
                    {
                        array_value.push_back(ubjson_read_internal(lpScript,ptr-ptrStart,ubj_type,&sh,err));
                        if(err)
                        {
                            goto exitlbl;
                        }                    
                        ptr+=sh;
                        if(ptr >= ptrEnd)
                        {
                            *err=MC_ERR_ERROR_IN_SCRIPT;
                            goto exitlbl;                            
                        }                          
                    }
                    ptr++;
                }
                result=array_value;
                break;
                
            case UBJ_OBJECT:
                
                ubj_type=UBJ_UNDEFINED;
                if(ptr >= ptrEnd)
                {
                    *err=MC_ERR_ERROR_IN_SCRIPT;
                    goto exitlbl;                            
                }  
                size=-1;
                if(*ptr == UBJ_TYPE[UBJ_STRONG_TYPE])
                {
                    ptr++;
                    if(ptr >= ptrEnd)
                    {
                        *err=MC_ERR_ERROR_IN_SCRIPT;
                        goto exitlbl;                            
                    }  
                    ubj_type=UBJ_INTERNAL_TYPE[*ptr];
                    if(ubj_type < 0)
                    {
                        *err=MC_ERR_ERROR_IN_SCRIPT;
                        goto exitlbl;        
                    }
                    if(ptr >= ptrEnd)
                    {
                        if(*ptr != UBJ_TYPE[UBJ_COUNT])
                        {
                            *err=MC_ERR_ERROR_IN_SCRIPT;
                            goto exitlbl;                                    
                        }
                    }
                }
                if(*ptr == UBJ_TYPE[UBJ_COUNT])
                {
                    ptr++;
                    size=(int)ubjson_int64_read(ptr,ptrEnd,UBJ_UNDEFINED,&sh,err);
                    if(err)
                    {
                        goto exitlbl;
                    }                    
                    ptr+=sh;
                }
                
                if(size >= 0)
                {
                    i=0;
                    while(i<size)
                    {
                        string_value=ubjson_read_internal(lpScript,ptr-ptrStart,UBJ_STRING,&sh,err);
                        if(err)
                        {
                            goto exitlbl;
                        }                    
                        ptr+=sh;

                        obj_value.push_back(Pair(string_value.get_str(),ubjson_read_internal(lpScript,ptr-ptrStart,ubj_type,&sh,err)));
                        if(err)
                        {
                            goto exitlbl;
                        }                    
                        ptr+=sh;
                        
                        i++;
                    }
                }
                else
                {
                    if(ptr >= ptrEnd)
                    {
                        *err=MC_ERR_ERROR_IN_SCRIPT;
                        goto exitlbl;                            
                    }  
                    while(*ptr != ']')
                    {
                        string_value=ubjson_read_internal(lpScript,ptr-ptrStart,UBJ_STRING,&sh,err);
                        if(err)
                        {
                            goto exitlbl;
                        }                    
                        ptr+=sh;

                        obj_value.push_back(Pair(string_value.get_str(),ubjson_read_internal(lpScript,ptr-ptrStart,ubj_type,&sh,err)));
                        if(err)
                        {
                            goto exitlbl;
                        }                    
                        ptr+=sh;
                        
                        if(ptr >= ptrEnd)
                        {
                            *err=MC_ERR_ERROR_IN_SCRIPT;
                            goto exitlbl;                            
                        }                          
                    }
                    ptr++;
                }
                result=array_value;
                break;
        }
    }
    
    
    
exitlbl:
                
    if(*err)
    {
        return Value::null;                        
    }

    return result;    
}

Value ubjson_read(mc_Script *lpScript,int offset,int *err)
{
    return ubjson_read_internal(lpScript,offset,UBJ_UNDEFINED,NULL,err);
}