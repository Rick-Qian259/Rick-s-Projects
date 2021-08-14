/*
created by:Qianxun Huai
ID:a1813515
time:19.09.2020

*/

#define _CRT_SECURE_NO_WARNINGS
#include <string>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <cctype>
#include <cstring>
#include <cstdio>
#include <cmath>
#include <algorithm>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <map>
#include <set>
#include <queue>
#include <stack>
#include <fstream>
#include <numeric>
#include <iomanip>
#include <bitset>
#include <list>
#include <stdexcept>
#include <functional>
#include <utility>
#include <ctime>

using namespace std;


int consumer_number;
int count_process;				// =1, first process, =2, second process
int wtq;						// weighted time quantum
int quota;						// ticket quota for every priority 
int total_time;					// system overall run time
int vec_original_size;			// queue1 original size
int vec2_original_size;			// queue2 original size
int current_run_time;
int total_input;
int push_where;					// =1, push into q1, otherwise =0


struct Consumer
{
	string ID;
	int arrival_time;
	int priority;
	int age;
	int require_tickets;
	int run_time;
	int ready_time;
	int wait_time;
	int end_time;
	int status;					// = 0, ready status, = 1, waiting status
	int buy_tickets;
};

vector <Consumer> input_data;	// store the input data	
vector <Consumer> input_data2;	// store the input data	2
vector <Consumer> vec;			// queue1
vector <Consumer> vec_2;		// queue2
/* every time program works, it will have some simultaneous arrival to queue1,
 *  store them in temp_store, then put them into the queue1 in terms of the order. */
vector <Consumer> result;		// output result


void initial()
{
	consumer_number = 0;
	count_process = 0;
	wtq = 0;
	quota = 0;
	total_time = 0;
	vec_original_size = 0;
	vec2_original_size = 0;
	current_run_time = 0;
	total_input = 0;
	push_where = 0;
}

void store(string id, int arr_time, int pri, int age, int tickets)
{
	Consumer co = { id ,arr_time ,pri ,age ,tickets };
	if (co.priority < 4)
	{
		input_data.push_back(co);
	}
	else
	{
		input_data2.push_back(co);
	}

}

/*
* Input ---> sort input[] ---> process(store into q1 & q2, delete stored input[], work) ---> print result
*/


void input()
{
	//string id;
	//int arr_time;
	//int pri;
	//int age; 
	//int tickets;
	string a1;
	int a2, a3, a4, a5;

	int i = 0;
	while (cin >> a1 >> a2 >> a3 >> a4 >> a5)
	{
		store(a1, a2, a3, a4, a5);
		i++;
	}

}

//sort the input[] in terms of thier arrival time.
void sort_input()
{
	int i, j;

	//sort due to arrival time first, if that same, then sort by priority.
	if (input_data.size() > 1)
	{
		for (i = 0; i < input_data.size() - 1; i++)
		{
			for (j = 0; j < input_data.size() - 1 - i; j++)
			{

				if (input_data[j].arrival_time > input_data[j + 1].arrival_time)
				{
					swap(input_data[j], input_data[j + 1]);
				}

				if (input_data[j].arrival_time == input_data[j + 1].arrival_time)
				{
					if (input_data[j].priority > input_data[j + 1].priority)
					{
						swap(input_data[j], input_data[j + 1]);
					}
				}

			}
		}
	}
	if (input_data2.size() > 1)
	{
		for (i = 0; i < input_data2.size() - 1; i++)
		{
			for (j = 0; j < input_data2.size() - 1 - i; j++)
			{
				if (input_data2[j].arrival_time > input_data2[j + 1].arrival_time)
				{
					swap(input_data2[j], input_data2[j + 1]);
				}

				if (input_data2[j].arrival_time == input_data2[j + 1].arrival_time)
				{
					if (input_data2[j].require_tickets > input_data2[j + 1].require_tickets)
					{
						swap(input_data2[j], input_data2[j + 1]);
					}
				}
			}
		}
	}


}

void store_single(Consumer co)
{
	int circle = 0;
	if (co.arrival_time <= total_time)
	{
		if (co.priority < 4)
		{
			if (vec.size() > 1)
			{
				for (int i = vec.size() - 1; i > 0; i--)
				{
					if (co.priority >= vec[i].priority)
					{
						vec.insert(vec.begin() + i + 1, co);
						circle = 0;
						break;
					}
					circle = 1;
				}
				if (circle == 1)
				{
					vec.insert(vec.begin(), co);
				}
			}
			else
			{
				vec.push_back(co);
			}

			//vec.push_back(co);
		}
		else    // put new arrival into vec_2.
		{
			vec_2.push_back(co);
		}
	}
}

void store_input()
{
	int i = 0;
	while (i < input_data.size() && input_data[i].arrival_time <= total_time)
	{
		//input_data[i].ready_time = 
		input_data[i].ready_time = total_time;
		store_single(input_data[i]);
		/*if (i < input_data.size()-1)
		{*/
		i++;
		//}

	}
	if (i != 0)
	{
		input_data.erase(input_data.begin(), input_data.begin() + i);

	}

	i = 0;
	while (i < input_data2.size() && input_data2[i].arrival_time <= total_time)
	{
		input_data2[i].ready_time = total_time;
		store_single(input_data2[i]);

		if (vec_2.size() > 1)
		{
			for (int j = 0; j < vec_2.size() - 1; j++)
			{
				for (int k = 0; k < vec_2.size() - 1; k++)
				{
					if (vec_2[k].require_tickets > vec_2[k + 1].require_tickets)
					{
						swap(vec_2[k], vec_2[k + 1]);
					}
				}
			}
		}
		/*if (i < input_data.size()-1)
		{*/
		i++;
		//}

	}
	if (i != 0)
	{
		input_data2.erase(input_data2.begin(), input_data2.begin() + i);

	}


}

void book()
{
	int push = 0;
	int circle = 0;
	Consumer temp_0;
	int temp_i = 0;


	/*
	* Queue 1
	* Every consumer has ticket quota due to the priority, if their current ticket requirment greater than quota,
	* current ticket requirment subtract quota, else current ticket requirment equol zero.
	* Every ticket will expend 5 time unit.
	*/
	current_run_time = 0;
	if (vec.size() != 0)
	{
		vec[0].buy_tickets = 0;
		while (vec.size() != 0)
		{
			if (push == 1)
			{
				break;
			}
			wtq = (10 - vec[0].priority) * 10;		// the formula in assignment
			quota = wtq / 5;

			vec[0].buy_tickets += 1;
			vec[0].require_tickets -= 1;
			vec[0].run_time += 5;
			vec[0].status = 1;
			current_run_time += 5;
			total_time += 5;
			count_process++;

			for (int n = 1; n < vec.size(); n++)
			{
				if (vec[n].status == 0)
				{
					vec[n].ready_time += 5;
				}
				else
				{
					vec[n].wait_time += 5;
				}
			}

			for (int n = 0; n < vec_2.size(); n++)
			{
				if (vec_2[n].status == 0)
				{
					vec_2[n].ready_time += 5;
				}
				else
				{
					vec_2[n].wait_time += 5;
				}
			}

			/*
			* Queue 2
			* When queue 1 has consumers, queue2's consumers should increase their age
			*/

			for (int i = 0; i < vec_2.size(); i++)
			{
				vec_2[i].age += 5;

				if (vec_2[i].age >= 100)
				{
					vec_2[i].age -= 100;
					vec_2[i].priority -= 1;
				}

				if (vec_2[i].priority < 4)
				{
					vec_2[i].age = 0;
					// *** C move from Q2 to Q1
					vec.push_back(vec_2[i]);
					vec_2.erase(vec_2.begin() + i);
					i--;
				}
			}

			// *** A new arrival to Q1
			if (input_data.size() != 0 || input_data2.size() != 0)
			{
				temp_0 = vec[0];
				store_input();
			}
			if (vec[0].ID != temp_0.ID)
			{
				for (int i = 1; i < vec.size(); i++)
				{
					if (temp_0.ID == vec[i].ID)
					{
						temp_i = i;
						break;
					}
				}
				vec.insert(vec.begin(), vec[temp_i]);
				vec.erase(vec.begin() + temp_i + 1);
			}

			if (vec[0].buy_tickets == quota && vec[0].require_tickets != 0)
			{
				push = 1;
				vec[0].age++;
				if (vec[0].age == 2)
				{
					vec[0].priority++;
					vec[0].age = 0;
				}

				if (vec[0].priority < 4)
				{
					// *** B move to the end of Q1
					for (int m = 1; m < vec.size(); m++)
					{
						if (vec[0].priority < vec[m].priority)
						{
							vec.insert(vec.begin() + m, vec[0]);
							vec.erase(vec.begin());
							circle = 0;
							break;
						}
						circle = 1;
					}
					if (circle == 1)
					{
						vec.push_back(vec[0]);
						vec.erase(vec.begin());
					}

					push_where = 1;
				}
			}


			if (vec[0].buy_tickets == quota && vec[0].require_tickets != 0)
			{
				if (vec[0].priority >= 4)
				{
					// *** B move Q1 to Q2
					vec_2.push_back(vec[0]);
					vec.erase(vec.begin());
					push_where = 0;

					if (vec_2.size() > 1)
					{
						for (int j = 0; j < vec_2.size() - 1; j++)
						{
							for (int k = 0; k < vec_2.size() - 1; k++)
							{
								if (vec_2[k].require_tickets > vec_2[k + 1].require_tickets)
								{
									swap(vec_2[k], vec_2[k + 1]);
								}
							}
						}
					}
				}
				if (vec.size() == 0)
				{
					break;
				}
			}
			// vec[0] will be delete from q1
			if (vec[0].require_tickets == 0)
			{
				vec[0].end_time = total_time;
				vec[0].wait_time = vec[0].end_time - vec[0].ready_time - vec[0].run_time;
				push_where = 0;
				result.push_back(vec[0]);
				vec.erase(vec.begin());
				break;
			}
		}
	}
	else
	{
		int i = 0;
		while (vec_2.size() != 0)
		{
			if (vec.size() != 0)
			{
				break;
			}
			vec_2[0].require_tickets -= 1;
			vec_2[0].run_time += 5;
			vec_2[0].status = 1;
			total_time += 5;
			current_run_time = 5;
			i++;
			// ready and waiting time 
			for (i = 1; i < vec_2.size(); i++)
			{
				if (vec_2[i].status == 0)
				{
					vec_2[i].ready_time += 5;
				}
				else
				{
					vec_2[i].wait_time += 5;
				}
			}
			// if q2's consumers age bigger than 100, then priority minus one and age become 0;
			for (int i = 1; i < vec_2.size(); i++)
			{
				vec_2[i].age += 5;
				if (vec_2[i].age >= 100)
				{
					vec_2[i].age -= 100;
					vec_2[i].priority -= 1;
				}

				if (vec_2[i].priority < 4)
				{
					vec.push_back(vec_2[i]);
					vec_2.erase(vec_2.begin() + i);
					i--;
				}
			}

			if (input_data2.size() != 0 && total_time == input_data2[0].arrival_time)
			{
				Consumer temp[1] = { vec_2[0] };

				store_input();

				for (int j = 0; j < vec_2.size() - 1; j++)
				{
					for (int k = 0; k < vec_2.size() - 1; k++)
					{
						if (vec_2[k].require_tickets > vec_2[k + 1].require_tickets)
						{
							swap(vec_2[k], vec_2[k + 1]);
						}
					}
				}
				if (temp[0].ID != vec_2[0].ID)
				{
					break;
				}
			}
			if (vec_2[0].require_tickets == 0)
			{
				vec_2[0].end_time = total_time;
				vec_2[0].wait_time = vec_2[0].end_time - vec_2[0].ready_time - vec_2[0].run_time;
				result.push_back(vec_2[0]);
				vec_2.erase(vec_2.begin());
				break;
			}
		}
	}

}

void print_detail()
{
	cout << "Time:" << total_time << endl;
	//name arrival_time tickets_required running priority_number age/runs
	cout << "name " << "arrival_time " << "tickets_required " << "running " << "priority_number " << "age/runs " << endl;
	cout << "queue1" << endl;
	int a = vec.size();
	int i;
	for (i = 0; i < a; i++)
	{
		cout << vec[i].ID << "\t" << vec[i].arrival_time << "\t" << vec[i].require_tickets << "\t"
			<< vec[i].run_time << "\t" << vec[i].priority << "\t" << vec[i].age << "\t" << vec[i].ready_time << "\t" << endl;
	}
	cout << "queue2" << endl;
	a = vec_2.size();
	for (i = 0; i < a; i++)
	{
		cout << vec_2[i].ID << "\t" << vec_2[i].arrival_time << "\t" << vec_2[i].require_tickets << "\t"
			<< vec_2[i].run_time << "\t" << vec_2[i].priority << "\t" << vec_2[i].age << "\t" << vec_2[i].ready_time << "\t" << endl;
	}
	cout << " " << endl;
}

void loop_()
{
	// if four vectors have not been empty, this program still works.
	while (vec.size() != 0 || vec_2.size() != 0 || input_data.size() != 0 || input_data2.size() != 0)
	{
		if (vec.size() == 0 && vec_2.size() == 0)
		{
			if (input_data.size() == 0)
			{
				total_time = input_data2[0].arrival_time;
			}
			else if (input_data2.size() == 0)
			{
				total_time = input_data[0].arrival_time;
			}
			else if (input_data.size() != 0 && input_data2.size() != 0)
			{
				if (input_data[0].arrival_time > input_data2[0].arrival_time)
				{
					total_time = input_data2[0].arrival_time;
				}
				else
				{
					total_time = input_data[0].arrival_time;
				}
			}
			store_input();
		}
		while (vec.size() != 0 || vec_2.size() != 0)
		{
			//print_detail();
			book();
		}
	}
}

//queue1
void print_queue()
{
	int a = vec.size();
	cout << "a:" << a << " queue1 " << "current time: " << total_time << endl;
	for (int i = 0; i < a; i++)
	{
		cout << i << " " << vec[i].ID + " " << vec[i].arrival_time << " " << vec[i].priority
			<< " " << vec[i].age << " " << vec[i].require_tickets << endl;
	}
	cout << endl;
}

//queue2
void print_queue2()
{
	int a = vec_2.size();
	cout << "a:" << a << " queue2 " << "current time: " << total_time << endl;
	for (int i = 0; i < a; i++)
	{
		cout << i << " " << vec_2[i].ID + " " << vec_2[i].arrival_time << " " << vec_2[i].priority
			<< " " << vec_2[i].age << " " << vec_2[i].require_tickets << endl;
	}
	cout << endl;
}

void print()		// input
{
	int a = input_data.size();
	int b = input_data2.size();
	cout << "a:" << a << " input " << "current time: " << total_time << endl;
	for (int i = 0; i < a; i++)
	{
		cout << i << " " << input_data[i].ID + " " << input_data[i].arrival_time << " " << input_data[i].priority
			<< " " << input_data[i].age << " " << input_data[i].require_tickets << endl;
	}
	cout << endl;

	cout << "b:" << b << " input2 " << "current time: " << total_time << endl;
	for (int i = 0; i < b; i++)
	{
		cout << i << " " << input_data2[i].ID + " " << input_data2[i].arrival_time << " " << input_data2[i].priority
			<< " " << input_data2[i].age << " " << input_data2[i].require_tickets << endl;
	}
	cout << endl;
}

void output()
{
	int i;
	cout << "name   arrival   end   ready   running   waiting" << endl;
	for (i = 0; i < result.size(); i++) // every result
	{
		cout << result[i].ID << "\t" << result[i].arrival_time << "\t" << result[i].end_time << "\t"
			<< result[i].ready_time << "\t" << result[i].run_time << "\t" << result[i].wait_time << "\t" << endl;
	}
	cout << endl;
}

void works()
{
	sort_input();
	loop_();
}

int main(int argc, char* argv[])
{
	initial();        // initial data
	freopen("input.txt", "r", stdin);
	input();          // input data
	works();
	output();         // output result
	return 0;
}










