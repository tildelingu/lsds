#ifndef UTILITIES_H
#define UTILITIES_H

long left_shift(long c, long n)
{
	// only for 8-bit char
	long result;
	long num;

	if (n < 0)
		return -1;

	for (result = c, num = n; num > 0; num = num - 1)
	{
		result = result * 2;
		if (result >= 256)
			result = result - 256;
	}

	return result;
}

long right_shift(long c, long n)
{
	long result;
	long num;

	if (n < 0)
		return -1;

	for (result = c, num = n; num > 0; num = num - 1)
	{
		result = result / 2;
	}

	return result;
}

long mod(long c, long n)
{
	// c % n
	long result;
	result = c - c/n*n;
	
	return result;
}

long base64_char2num(long ch)
{
	long num;
	long flag_correct;
	flag_correct = 0;

	if(ch >= 'A')
	{
		if (ch <= 'Z')
		{
			num = ch - 'A';
			flag_correct = 1;
		}
	}
	
	if(ch >= 'a')
	{
		if (ch <= 'z')
		{
			num = ch - 'a' + 26;
			flag_correct = 1;
		}
	}
	
	if(ch >= '0')
	{
		if (ch <= '9')
		{
			num = ch - '0' + 52;
			flag_correct = 1;
		}
	}
	
	if(ch == '+')
	{
		num = 62;
		flag_correct = 1;
	}
	
	if(ch == '/')
	{
		num = 63;
		flag_correct = 1;
	}
	
	if (flag_correct == 0)
	{
		num = -1;
	}

	return num;
}

void error_msg()
{
	output_char('E');
	output_char('n');
	output_char('c');
	output_char('o');
	output_char('d');
	output_char('e');
	output_char('d');
	output_char(' ');
	output_char('f');
	output_char('i');
	output_char('l');
	output_char('e');
	output_char(' ');
	output_char('c');
	output_char('u');
	output_char('r');
	output_char('r');
	output_char('u');
	output_char('p');
	output_char('t');
	output_char('e');
	output_char('d');
	output_char('!');
	output_char(10);

	return;
}

#endif
