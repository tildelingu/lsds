#include "libi0/malloc.h"
#include "libi0/stdio.h"
#include "utilities.h"

#define N_LONG_PAGE 4096

void watcher1(long* data_de1, long data_de1_len)
{
	long i;
	
	if (data_de1[0] != 0)
	{
		for (i=0; i<=data_de1_len; i=i+1)
		{
			output_char(data_de1[i]);
		}
		output_char(10);

		commitd;
	}
	
	abort;
}

void watcher2(long* data_de1, long data_de1_len, long* data_de2, long data_de2_len)
{
	long i;
	
	if (data_de1[0] != 0)
	{
		if (data_de2[0] != 0)
		{
			for (i=0; i<=data_de1_len; i=i+1)
			{
				output_char(data_de1[i]);
			}
			for (i=0; i<data_de2_len; i=i+1)
			{
				output_char(data_de2[i]);
			}
			output_char(10);

			commitd;		
		}
	}
	
	abort;
}

void decode_runner(long* data_en, long begin_en, long end_en, long* data_de, long data_de_len)
{
	long i_en;
	long i_de;
	
	for(i_en = begin_en, i_de = 0 ; i_en<end_en ; i_en = i_en + 4)
	{
		data_de[i_de] = left_shift(base64_char2num(data_en[i_en]), 2) + right_shift(base64_char2num(data_en[i_en+1]), 4);
		
		if(data_en[i_en+2] != '=')
		{
			data_de[i_de+1] = left_shift(base64_char2num(data_en[i_en+1]), 4) + right_shift(base64_char2num(data_en[i_en+2]), 2);
		}
		else
		{
			break;
		}
		
		if(data_en[i_en+3] != '=')
		{
			data_de[i_de+2] = left_shift(base64_char2num(data_en[i_en+2]), 6) + base64_char2num(data_en[i_en+3]);
		}
		else
		{
			break;
		}
		
		i_de = i_de + 3;
	}
	
	commit;
}

void init_runner()
{
	long len_en;
	long* data_en;
	long len_de;
	long len_de_actual;
	long partition_en;
	long partition_de;
	long* data_de1;
	long* padding;
	long* data_de2;
	long data_de1_len;
	long data_de2_len;
	long flag_corrupted;
	long i;
	
	flag_corrupted = 0;
	
	len_en = input_q(); // size of encoded data
	
	if (len_en < 4)
	{
		flag_corrupted = 1;
	}
	else if (mod(len_en, 4) != 0)
	{
		flag_corrupted = 1;
	}
	else
	{
		// read in encoded data
    	data_en = (long*)shalloc(len_en * 8);
    	for (i=0; i<len_en; i=i+1)
    	{
    		data_en[i] = (long)input_char();
    		if (data_en[i] == '=')
    		{
    			if (i != len_en-1)
    			{
    				if (i != len_en-2)
    				{
    					flag_corrupted = 1;
    					break;
    				}
    			}
    		}
    		else if (base64_char2num(data_en[i]) == -1)
    		{
    			flag_corrupted = 1;
    			break;
    		}
    	}
    }
    
    if (flag_corrupted == 1)
    {
    	error_msg();
    	abort;
    }
    else
    {
    	len_de = len_en/4*3;
    	// calculate actual size of decoded data
    	if(data_en[len_en-1] == '=')
		{
			if (data_en[len_en-2] == '=')
			{
				len_de_actual = len_de - 2;
			}
			else
			{
				len_de_actual = len_de - 1;
			}
		}
		else
		{
			len_de_actual = len_de;
		}
    	
    	// decode
    	if (len_en == 4)
    	{
    		data_de1 = (long*)shalloc(3 * 8);
    		runner decode_runner(data_en, 0, 4, data_de1, 3)
				using data_en[0,, 4], data_de1[0,, 3];
			
			runner watcher1(data_de1, 3)
				using data_de1[0,, 3]
				watching data_de1[0,, 3];
    	}
    	else
    	{
    		// encoded data partition:
    		// data_en[0,, partition_en]
    		// data_en[partition_en,, len_en]
    		partition_en = (len_en/2) - mod((len_en/2), 4);
    		// decoded data partition:
    		// data_de1[0,, partition_de]
    		// data_de2[0,, len_de_actual-partition_de]
    		partition_de = (partition_en)/4*3;
			data_de1_len = partition_de;
			data_de2_len = len_de_actual - partition_de;

			data_de1 = (long*)shalloc(data_de1_len * 8);
			padding = (long*)shalloc(N_LONG_PAGE * 8);
			data_de2 = (long*)shalloc(data_de2_len * 8);
		
			runner decode_runner(data_en, 0, partition_en, data_de1, data_de1_len)
				using data_en[0,, partition_en], data_de1[0,, data_de1_len];
			
			runner decode_runner(data_en, partition_en, len_en, data_de2, data_de2_len)
				using data_en[partition_en,, len_en], data_de2[0,, data_de2_len];
			
			runner watcher2(data_de1, data_de1_len, data_de2, data_de2_len)
				using data_de1[0,, data_de1_len], data_de2[0,, data_de2_len]
				watching data_de1[0,, data_de1_len], data_de2[0,, data_de2_len];
		}
		
		commit;
    }
}

void main()
{
	runner init_runner();

    commit;
}
