// Copyright (c) 2003-2014, LogMeIn, Inc. All rights reserved.
// This is part of Xively C library, it is under the BSD 3-Clause license.

#include "xi_stated_sscanf.h"
#include "xi_coroutine.h"
#include "xi_stated_sscanf_helpers.h"

#ifdef __cplusplus
extern "C" {
#endif

signed char xi_stated_sscanf(
          xi_stated_sscanf_state_t* s
        , const const_data_descriptor_t* pattern
        , const_data_descriptor_t* source
        , void** variables )
{
    // internal tmp variables
    signed char res     = 0;
    data_descriptor_t dst_desc;
    
    BEGIN_CORO( s->state )

    s->vi = 0;
    s->p  = 0;

    if( source->curr_pos == source->real_size ) // check if the data not needed
    {
        YIELD( s->state, 0 );
        source->curr_pos = 0;
    }

    for( ; s->p < pattern->real_size; )
    {
        if( pattern->data_ptr[ s->p ] != '%' ) // check on the raw pattern basis one to one
        {
            if( pattern->data_ptr[ s->p ] != source->data_ptr[ source->curr_pos ] )
            {
                EXIT( s->state, -1 );
            }
            else
            {
                source->curr_pos++;
                s->p++;

                if( source->curr_pos == source->real_size )
                {
                    // accepting should have bigger priority than more data
                    if( s->p == pattern->real_size )
                    {
                        goto accept;
                    }
                    else
                    {
                        YIELD( s->state, 0 )
                        source->curr_pos = 0;
                        continue;
                    }
                }
            }
        }
        else // parsing state
        {
            // simplified version so don't expect to parse %%
            s->p++;             // let's move the marker to the type or constraint
            s->max_len = 255;   // let's clean the max len

            if( is_digit( pattern->data_ptr[ s->p ] ) )
            {
                s->buff_len = 0;

                {
                    dst_desc.data_ptr   = s->buffer;
                    dst_desc.data_size  = sizeof( s->buffer ); 
                    dst_desc.real_size  = sizeof( s->buffer );
                    dst_desc.curr_pos   = 0;
                    
                    res = safe_until_copier( &s->buff_len, &s->p, &dst_desc, pattern, &is_digit );
                }

                if( res == -1 )
                {
                    EXIT( s->state, -1 );
                }

                to_unsigned_char( s, ( unsigned char* ) &( s->max_len ) );
            }


            // @TODO think of this implementation and take into concideration
            // an idea of putting more of each case implementation into the common function
            // that would than be reused and make the code smaller
            if( pattern->data_ptr[ s->p ] == 'd' )
            {
                s->buff_len = 0;

                do
                {
                    dst_desc.data_ptr   = s->buffer;
                    dst_desc.data_size  = sizeof( s->buffer ); 
                    dst_desc.real_size  = sizeof( s->buffer );
                    dst_desc.curr_pos   = 0;
                    
                    
                    res = safe_until_copier( &s->buff_len, &source->curr_pos, &dst_desc, source, &is_digit );
                    if( res == 0 )
                    {
                        YIELD( s->state, 0 )
                        source->curr_pos = 0;
                    }
                } while( res == 0 );

                to_int( s, variables[ s->vi ] );

                s->p++;     // move on, finished with parsing
                s->vi++;    // switch to the next variable
            }
            else if(   pattern->data_ptr[ s->p ] == 's'
                    || pattern->data_ptr[ s->p ] == '.'
                    || pattern->data_ptr[ s->p ] == 'B'
                    || pattern->data_ptr[ s->p ] == 'C' )
            {
                s->tmp_i = 0;

                do
                {                    
                    dst_desc.data_ptr   = ( char* ) variables[ s->vi ];
                    dst_desc.data_size  = s->max_len; 
                    dst_desc.real_size  = s->max_len - 1;
                    dst_desc.curr_pos   = 0;


                    switch( pattern->data_ptr[ s->p ] )
                    {
                        case 's':
                            res = safe_until_copier( &s->tmp_i, &source->curr_pos, &dst_desc, source, &is_header );
                            break;
                        case '.':
                            res = safe_until_copier( &s->tmp_i, &source->curr_pos, &dst_desc, source, &is_any );
                            break;
                        case 'B':
                            res = safe_until_copier( &s->tmp_i, &source->curr_pos, &dst_desc, source, &pass_all );
                            break;
                        case 'C':
                            res = safe_until_copier( &s->tmp_i, &source->curr_pos, &dst_desc, source, &is_channel_id );
                            break;
                    }

                    if( res == 0 )
                    {
                        YIELD( s->state, 0 )
                        source->curr_pos = 0;
                    }
                } while( res == 0 );

                ( ( char* ) variables[ s->vi ] )[ s->tmp_i ] = '\0'; // put guard

                s->p++;     // move on, finished with parsing
                s->vi++;    // switch to the next variable
            }
        }
    }

accept:
    RESTART( s->state, 1 )

    END_CORO()
}

#ifdef __cplusplus
}
#endif
