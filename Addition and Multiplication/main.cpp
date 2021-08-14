#include <iostream>
#include <string>
using namespace std;

string Addition(string str1, string str2, int base)
{
    int length_diff;
	/*
	if two numbers have different length, first we need to address the short number with 0 in front of it,
	and make it have the same length with long number;
	*/
	if (str1.size() != str2.size())
	{
		if (str1.size() < str2.size()) // make str1 length >= str2 length in all cases
		{
			string swap = str1;
			str1 = str2;
			str2 = swap;
		}
		//add 0 in front of the str2
		length_diff = str1.size() - str2.size();
		for (int i = 0; i < length_diff; i++)
		{
			str2 = '0' + str2;
		}
	}
	int carry = 0;
	string sum = "";
	int tmp = 0;
	//add two numbers from right digit to the left digit
	for (int i = str1.size() - 1; i >= 0; i--)
	{
		tmp = (str1.at(i) - '0') + (str2.at(i) - '0') + carry;
		if (tmp >= base) {
			tmp = tmp - base;
			carry = 1;
		}
		else
		{
			carry = 0;
		}
		sum = to_string(tmp) + sum;
	}
	//for the most significant digit, if carry is one, it must add '1' in the head.
	if (carry == 1)
	{
		sum = '1' + sum;
	}
	return sum;
}

string Subtraction(string str1, string str2, int base)
{
	int length_diff = 0;
	//add the 0 in front of the short number, make two number have same digit
	if (str1.size() != str2.size())
	{
		length_diff = str1.size() - str2.size();

	}
	if (length_diff < 0)
	{
		length_diff = -1 * length_diff;
	}
	for (int i = 0; i < length_diff; i++)
	{
		if (str1.size() > str2.size()) {	//str2 is short, and add '0'
			str2 = '0' + str2;
		}
		else  //str1 is short, and add '0'
		{
			str1 = '0' + str1;
		}
	}
	int carry = 0;
	string difference = "";
	int tmp = 0;
	for (int i = str1.size() - 1; i >= 0; i--)
	{
		tmp = (str1.at(i) - 48) - (str2.at(i) - 48) - carry;	// -48 because star1.at(i) is a char, and 48 is the '0' ascii 
		if (tmp < 0)
		{
			tmp = tmp + base;
			carry = 1;
		}
		else
		{
			carry = 0;
		}
		difference = to_string(tmp) + difference;
	}
	return difference;
}
// this method is used to add zero in the first part of number. 
// for example, 1531, a1 =15, and it needs to add two zero in the back
string addZero(string str1, int base)
{
	for (int i = 0; i < base; i++)
	{
		str1 = str1 + "0";
	}
	return str1;
}

//the recurive method to multiply the number
string karatsuba(string str1, string str2, int base)
{
	//basic case
	if (str1.size() == 1 && str2.size() == 1)
	{
		int int_1 = std::stoi(str1);
		string res = "";
		// for example, 6*7  and Base = 8, it means add 6 seven times
		for (int i = 0; i < int_1; i++)
		{
			res = Addition(res, str2, base);
		}
		return res;
	}
	else
	{
		string a0 = "", a1 = "";
		string b0 = "", b1 = "";
		int length_diff = 0; 
		//same method wit the Addition for add zero
		if (str1.size() != str2.size())
		{
			if (str1.size() < str2.size()) // make str1 length >= str2 length in all cases
			{
				string swap = str1;
				str1 = str2;
				str2 = swap;
			}
			//add 0 in front of the str2
			length_diff = str1.size() - str2.size();
			for (int i = 0; i < length_diff; i++)
			{
				str2 = '0' + str2;
			}
		}
		int length = str1.size();
		int k = length / 2;
		// a = a1*B^k + a0
		// split a1,b1
		for (int i = 0; i < length - k; i++)
		{
			a1 = a1 + str1.at(i);
			b1 = b1 + str2.at(i);
		}
		//split a0,b0
		for (int i = length - k; i < length; i++)
		{
			a0 = a0 + str1.at(i);
			b0 = b0 + str2.at(i);
		}
		/*
		p = p2 * B^2k + (p1-(p2+p0)) * B^k + p0
			p2 = a1*b1;
			p1 = (a1+a0)(b1+b0);
			p0 = a0*b0;

		rest2 = p2 * B^2k
		rest1 = p1-(p2+p0)) * B^k
		So, p = rest2 + rest1 + p0
		*/
		string p2, p1, p0;
		p2 = karatsuba(a1, b1, base);
		string tmp1 = Addition(a1, a0, base);
		string tmp2 = Addition(b1, b0, base);
		p1 = karatsuba(tmp1, tmp2, base);
		p0 = karatsuba(a0, b0, base);
		string rest2 = addZero(p2, 2*k);
		string res1 = Addition(p2, p0, base);
		string res2 = Subtraction(p1, res1, base);
		string rest1 = addZero(res2, k);
		string result1 = Addition(rest2, rest1, base);
		string result = Addition(result1, p0, base);
		return result;
	}
}
/*
when execute the karatsuba method, it will have the resutl with the '0' in the head, 
so this method is used to delete the '0' in front the number, for example, 
the result is 004433 in karatsuba method, this method can delete the 00 to 4433.
*/
string solve(string str1, string str2, int base)
{
	string s = karatsuba(str1, str2, base);
	int i;
	for (i = 0; i < s.size(); i++)
	{
		if (s.at(i) != '0')
		{
			break;
		}
	}
	s = s.substr(i);
	return s;
}

int main(){
	string I1, I2;
	int B;
	cin >> I1 >> I2 >> B;
	string add, mul;
	add = Addition(I1, I2, B);
	mul = solve(I1, I2, B);
	cout <<  add << " " << mul << endl;
	return 0;
}

