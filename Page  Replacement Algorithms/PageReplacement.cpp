/*
OS assignment2
created by:Qianxun Huai
ID:a1813515
time:16.10.2020

*/

#define _CRT_SECURE_NO_WARNINGS
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <bitset>


using namespace std;


int events_num;
string events_name;
int pagesize;
int frame;
int disk_read;
int disk_write;
int reg_num;
int interval;
int win_size;

void initial()
{
    events_num = 0;
    pagesize = 0;
    frame = 0;
    disk_read = 0;
    disk_write = 0;
    reg_num = 0;
    interval = 0;
    win_size = 0;
}


struct Trace
{
    string status;			// read or write
    string memory;			// hexdecimal memory address
    int page_num;			// allocate the page
    string reg;				// register for ARB
    int frequency;			// frequency in the working set

};


vector <Trace> input_data;			// store each trace from input.txt
vector <Trace> memory_data;			//
vector <Trace> ws_data;				// working set data



// used to calculate the power of 16
int powerOf16(int n)
{
    if (n == 0)
    {
        return 1;
    }
    else if (n == 1)
    {
        return 16;
    }
    else
    {
        return 16 * powerOf16(n - 1);
    }
}

//convert memory address from hexadecimal into decimal
int hexToDec(string str)
{
    int i, a;
    int l = str.length();
    int result = 0;

    for (i = l - 1; i >= 0; i--)
    {
        if (str.at(i) > 96)						// char 'a' == 97
        {
            a = str.at(i) - 97 + 10;
        }
        else									// char '0' == 48
        {
            a = str.at(i) - 48;
        }
        result = result + a * powerOf16(l - i - 1);
    }

    return result;

}

//store the data from input file into a vector
void store(string status, string memory, int page_num, string reg)
{
    Trace tr = { status , memory, page_num, reg };
    input_data.push_back(tr);
}

void input()
{
    string str1, str2, str3 = "";
    int dec;
    int pagenum;

    int i = 0;
    cin >> str1 >> str2;
    events_name = str2;

    while (cin >> str1 >> str2)
    {
        dec = hexToDec(str2);
        pagenum = dec / pagesize;

        if (reg_num > 0)						//create register for ARB trace
        {
            for (int j = 0; j < reg_num; j++)
            {
                str3 = "0" + str3;
            }
        }
        store(str1, str2, pagenum, str3);
        i++;
        str3 = "";
    }

    events_num = i;

}

void printFrame(vector <Trace> input)
{
    cout << "frame: ";
    for (int i = 0; i < input.size(); i++)
    {
        cout << input[i].page_num << "(" << input[i].reg << ")    ";
    }
    cout << endl;
}

/*
there is one issues, if frame like this : 4 8(R) 1 3 ;
next trace is 8(W), if I just replace the 8's (R) into (W), but don't replace it address,
it will have some trouble in some unknown cases?
*/

void FIFO()
{
    int i, j, temp;
    bool read = true;
    for (i = 0; i < events_num; i++)
    {
        if (memory_data.size() < frame)
        {
            if (memory_data.size() > 0)
            {
                for (j = 0; j < memory_data.size(); j++)
                {
                    if (input_data[i].page_num == memory_data[j].page_num)
                    {
                        read = false;
                        temp = j;
                        break;
                    }
                }

                if (read)
                {
                    memory_data.push_back(input_data[i]);
                    disk_read++;
                }
                else
                {
                    if (input_data[i].status == "W")
                    {
                        memory_data[temp].status = input_data[i].status;
                    }

                }
                read = true;
            }
            else				// the first trace
            {
                memory_data.push_back(input_data[i]);
                disk_read++;
            }
        }
        else				// replacement process	
        {
            // search if the new trace in the FIFO, if yes, don't read
            for (j = 0; j < frame; j++)
            {
                if (input_data[i].page_num == memory_data[j].page_num)
                {
                    read = false;
                    break;
                }
            }
            if (read)				//add to the back and delete the head
            {
                if (memory_data[0].status == "W")
                {
                    disk_write++;
                }
                memory_data.push_back(input_data[i]);
                memory_data.erase(memory_data.begin());
                disk_read++;
            }
            else					// if the new in the memory vector, don't insert the new
            {
                if (memory_data[j].status == "R" && input_data[i].status != memory_data[j].status)
                {
                    memory_data[j].status = input_data[i].status;
                }
            }
            read = true;
        }
    }
}

void LRU()
{
    int i, j, temp;
    bool del = false;
    for (i = 0; i < events_num; i++)
    {
        if (memory_data.size() < frame)
        {
            for (j = 0; j < memory_data.size(); j++)
            {
                if (input_data[i].page_num == memory_data[j].page_num)
                {
                    del = true;
                    temp = j;
                    break;
                }
            }
            if (del)
            {
                if (memory_data[temp].status == "W" && input_data[i].status == "R")
                {
                    input_data[i].status = "W";
                }
                memory_data.erase(memory_data.begin() + temp);
                memory_data.push_back(input_data[i]);
                del = false;
            }
            else				// the first trace
            {
                memory_data.push_back(input_data[i]);
                disk_read++;
            }
        }
        else					// replacement process				
        {
            for (j = 0; j < frame; j++)
            {
                if (input_data[i].page_num == memory_data[j].page_num)
                {
                    del = true;
                    temp = j;
                    break;
                }
            }
            if (del)				// the new trace is in the memory, insert the new and delete the old
            {
                if (memory_data[temp].status == "W" && input_data[i].status == "R")
                {
                    input_data[i].status = "W";
                }
                memory_data.erase(memory_data.begin() + temp);
                memory_data.push_back(input_data[i]);
                del = false;
            }
            else					// the new doesn't in the memory, insert
            {
                if (memory_data[0].status == "W")
                {
                    disk_write++;
                }
                memory_data.push_back(input_data[i]);
                memory_data.erase(memory_data.begin());
                disk_read++;
            }
        }
    }
}

void ARB()
{
    int i, temp;
    int inter_count = 0;
    bool read = true;
    for (i = 0; i < events_num; i++)
    {

        if (memory_data.size() < frame)			// before the replace process
        {
            int j;
            for (j = 0; j < memory_data.size(); j++)
            {
                if (input_data[i].page_num == memory_data[j].page_num)
                {
                    read = false;
                    temp = j;
                    break;
                }
            }
            if (inter_count == interval)			// if the period equal the interval, deal all memory's Register 
            {
                for (int k = 0; k < memory_data.size(); k++)
                {
                    memory_data[k].reg.insert(0, "0");
                    memory_data[k].reg.erase(memory_data[k].reg.size() - 1);
                }
                inter_count = 0;
            }

            if (read)				//read the new
            {
                input_data[i].reg.replace(0, 1, "1");
                memory_data.push_back(input_data[i]);
                disk_read++;
            }
            else					//replace the previous
            {
                if (input_data[i].status == "W")
                {
                    memory_data[temp].status = input_data[i].status;
                }
                memory_data[temp].reg.replace(0, 1, "1");
            }
            inter_count++;
            read = true;
        }
        else					// after the replace process
        {
            // search if the new trace in the FIFO, if yes, don't read
            int j;
            for (j = 0; j < frame; j++)
            {
                if (input_data[i].page_num == memory_data[j].page_num)
                {
                    read = false;
                    temp = j;
                    break;
                }
            }

            if (inter_count == interval)			// if the period equal the interval, deal all memory's Register 
            {
                for (int k = 0; k < memory_data.size(); k++)
                {
                    memory_data[k].reg.insert(0, "0");
                    memory_data[k].reg.erase(memory_data[k].reg.size() - 1);
                }
                inter_count = 0;
            }

            if (read)						//add to the back and delete the head
            {

                // find which one should be deleted
                int min = 0;
                for (int k = 0; k < memory_data.size(); k++)
                {
                    if (memory_data[min].reg > memory_data[k].reg)
                    {
                        min = k;
                    }
                }

                if (memory_data[min].status == "W")
                {
                    disk_write++;
                }

                input_data[i].reg.replace(0, 1, "1");
                memory_data.push_back(input_data[i]);
                memory_data.erase(memory_data.begin() + min);
                disk_read++;
            }
            else					// if the new in the memory vector, don't insert the new
            {
                memory_data[temp].reg.replace(0, 1, "1");
                if (memory_data[j].status == "R" && input_data[i].status != memory_data[j].status)
                {
                    memory_data[j].status = input_data[i].status;
                }

            }
            inter_count++;
            read = true;
        }
    }
}

void WSARB_1()
{
    int i, temp;
    int inter_count = 0;
    bool read = true;
    int front = 0;
    int end = 0;
    bool equal = false;
    /*
    1 2 3 4 5 6
    1 5 3 8 7 6
    E    	  F
    F-E = 6, window size = 6
    */
    for (i = 0; i < events_num; i++)
    {
        ws_data.clear();
        if (memory_data.size() < frame)			// before the replace process
        {
            int j;
            for (j = 0; j < memory_data.size(); j++)
            {
                if (input_data[i].page_num == memory_data[j].page_num)
                {
                    read = false;
                    temp = j;
                    break;
                }
            }
            if (inter_count == interval)			// if the period equal the interval, deal all memory's Register 
            {
                for (int k = 0; k < memory_data.size(); k++)
                {
                    memory_data[k].reg.insert(0, "0");
                    memory_data[k].reg.erase(memory_data[k].reg.size() - 1);
                }
                inter_count = 0;
            }

            if (read)				//read the new
            {
                input_data[i].reg.replace(0, 1, "1");
                memory_data.push_back(input_data[i]);
                disk_read++;
                front = i;				// front in the last indicator of memory vector
                end = front - win_size + 1;
                if (end < 0)
                {
                    end = 0;
                }
            }
            else					//replace the previous
            {
                if (input_data[i].status == "W")
                {
                    memory_data[temp].status = input_data[i].status;
                }
                memory_data[temp].reg.replace(0, 1, "1");
            }
            inter_count++;
            read = true;
        }
        else					// after the replace process
        {
            // search if the new trace in the memory, if yes, don't read
            int j;
            for (j = 0; j < frame; j++)
            {
                if (input_data[i].page_num == memory_data[j].page_num)
                {
                    read = false;
                    temp = j;
                    break;
                }
            }
            // if the period equal the interval, deal all memory's Register 
            if (inter_count == interval)
            {
                for (int k = 0; k < memory_data.size(); k++)
                {
                    memory_data[k].reg.insert(0, "0");
                    memory_data[k].reg.erase(memory_data[k].reg.size() - 1);
                }
                inter_count = 0;
            }

            if (read)						//add to the back and delete the head
            {
                /*
                   the read order is that, first push the current trace to the back of vector, then find which trace should be deleted.
                   the delete order is that, first find the smallest refrence frequency,
                   if it equal, then find the smallest ARB shift register.

                */
                input_data[i].reg.replace(0, 1, "1");
                memory_data.push_back(input_data[i]);
                front = i;				// front in the last indicator of memory vector
                end = front - win_size + 1;
                for (int k = end; k <= front; k++)
                {
                    ws_data.push_back(input_data[k]);
                }
                //sort
                for (int m = 0; m < ws_data.size() - 1; m++)
                {
                    for (int n = 0; n < ws_data.size() - m - 1; n++)
                    {
                        if (ws_data[n].page_num > ws_data[n + 1].page_num)
                        {
                            swap(ws_data[n], ws_data[n + 1]);
                        }
                    }
                }
                int count = 1;
                //store the frequency of each trace in memory_data
                for (int k = 0; k < ws_data.size(); k++)
                {
                    if (k < ws_data.size() - 1 && ws_data[k].page_num != ws_data[k + 1].page_num)
                    {
                        for (int l = 0; l < memory_data.size() - 1; l++)
                        {
                            if (memory_data[l].page_num == ws_data[k].page_num)
                            {
                                memory_data[l].frequency = count;
                                break;
                            }
                        }
                        count = 1;
                        if (k == ws_data.size() - 2)
                        {
                            for (int l = 0; l < memory_data.size() - 1; l++)
                            {
                                if (memory_data[l].page_num == ws_data[k + 1].page_num)
                                {
                                    memory_data[l].frequency = 1;
                                    count = 1;
                                }
                            }
                        }
                    }
                    else if (k == ws_data.size() - 2 && ws_data[k].page_num == ws_data[k + 1].page_num)
                    {
                        count++;
                        for (int l = 0; l < memory_data.size(); l++)
                        {
                            if (memory_data[l].page_num == ws_data[k].page_num)
                            {
                                memory_data[l].frequency = count;
                                break;
                            }
                        }
                    }
                    else
                    {
                        count++;
                    }
                }
                int min = 0;
                int temp_min = 0;
                //find which one should be deleted

                //find the smallest frequency
                for (int k = 0; k < memory_data.size() - 1; k++)
                {
                    if (memory_data[min].frequency > memory_data[k].frequency)
                    {
                        min = k;
                    }
                }
                temp_min = min;
                int count_min = 0;
                //search the times of the smallest frequency, if it greater than 1, find the smallest register .
                for (int k = 0; k < memory_data.size() - 1; k++)
                {
                    if (memory_data[k].frequency == memory_data[temp_min].frequency)
                    {
                        count_min++;
                    }
                }
                if (count_min > 1)
                {
                    for (int k = 0; k < memory_data.size() - 1; k++)
                    {
                        if (memory_data[k].frequency == memory_data[temp_min].frequency && memory_data[min].reg > memory_data[k].reg)
                        {
                            min = k;
                        }
                    }
                }


                if (memory_data[min].status == "W")
                {
                    disk_write++;
                }
                memory_data.erase(memory_data.begin() + min);
                disk_read++;
            }
            else					// if the new in the memory vector, don't insert the new
            {
                memory_data[temp].reg.replace(0, 1, "1");
                if (memory_data[j].status == "R" && input_data[i].status != memory_data[j].status)
                {
                    memory_data[j].status = input_data[i].status;
                }

            }
            inter_count++;
            read = true;
        }
        for (int l = 0; l < memory_data.size(); l++)
        {
            memory_data[l].frequency = 0;
        }
    }
}

void WSARB_2()
{
    int i, temp;
    int inter_count = 0;
    bool read = true;
    int front = 0;
    int end = 0;
    bool equal = false;
    /*
    1 2 3 4 5 6
    1 5 3 8 7 6
    E    	  F
    F-E = 6, window size = 6
    */
    for (i = 0; i < events_num; i++)
    {
        ws_data.clear();
        if (memory_data.size() < frame)			// before the replace process
        {
            int j;
            for (j = 0; j < memory_data.size(); j++)
            {
                if (input_data[i].page_num == memory_data[j].page_num)
                {
                    read = false;
                    temp = j;
                    break;
                }
            }
            if (inter_count == interval)			// if the period equal the interval, deal all memory's Register 
            {
                for (int k = 0; k < memory_data.size(); k++)
                {
                    memory_data[k].reg.insert(0, "0");
                    memory_data[k].reg.erase(memory_data[k].reg.size() - 1);
                }
                inter_count = 0;
            }

            if (read)				//read the new
            {
                input_data[i].reg.replace(0, 1, "1");
                memory_data.push_back(input_data[i]);
                disk_read++;
                front = i;				// front in the last indicator of memory vector
                end = front - win_size + 1;
                if (end < 0)
                {
                    end = 0;
                }
            }
            else					//replace the previous
            {
                if (input_data[i].status == "W")
                {
                    memory_data[temp].status = input_data[i].status;
                }
                memory_data[temp].reg.replace(0, 1, "1");
            }
            inter_count++;
            read = true;
        }
        else					// after the replace process
        {
            // search if the new trace in the memory, if yes, don't read
            int j;
            for (j = 0; j < frame; j++)
            {
                if (input_data[i].page_num == memory_data[j].page_num)
                {
                    read = false;
                    temp = j;
                    break;
                }
            }
            // if the period equal the interval, deal all memory's Register 
            if (inter_count == interval)
            {
                for (int k = 0; k < memory_data.size(); k++)
                {
                    memory_data[k].reg.insert(0, "0");
                    memory_data[k].reg.erase(memory_data[k].reg.size() - 1);
                }
                inter_count = 0;
            }

            if (read)						//add to the back and delete the head
            {
                /*
                   the read order is that, first push the current trace to the back of vector, then find which trace should be deleted.
                   the delete order is that, first find the smallest refrence frequency,
                   if it equal, then find the smallest ARB shift register.

                */

                input_data[i].reg.replace(0, 1, "1");
                memory_data.push_back(input_data[i]);

                front = i;				// front in the last indicator of memory vector
                end = front - win_size + 1;

                for (int k = end; k <= front; k++)
                {
                    ws_data.push_back(input_data[k]);
                }
                //sort
                for (int m = 0; m < ws_data.size() - 1; m++)
                {
                    for (int n = 0; n < ws_data.size() - m - 1; n++)
                    {
                        if (ws_data[n].page_num > ws_data[n + 1].page_num)
                        {
                            swap(ws_data[n], ws_data[n + 1]);
                        }
                    }
                }
                int count = 1;
                //store the frequency of each trace in memory_data
                for (int k = 0; k < ws_data.size(); k++)
                {
                    if (k < ws_data.size() - 1 && ws_data[k].page_num != ws_data[k + 1].page_num)
                    {
                        for (int l = 0; l < memory_data.size() - 1; l++)
                        {
                            if (memory_data[l].page_num == ws_data[k].page_num)
                            {
                                memory_data[l].frequency = count;
                                break;
                            }
                        }
                        count = 1;
                        if (k == ws_data.size() - 2)
                        {
                            for (int l = 0; l < memory_data.size() - 1; l++)
                            {
                                if (memory_data[l].page_num == ws_data[k + 1].page_num)
                                {
                                    memory_data[l].frequency = 1;
                                    count = 1;
                                }
                            }
                        }
                    }
                    else if (k == ws_data.size() - 2 && ws_data[k].page_num == ws_data[k + 1].page_num)
                    {
                        count++;
                        for (int l = 0; l < memory_data.size(); l++)
                        {
                            if (memory_data[l].page_num == ws_data[k].page_num)
                            {
                                memory_data[l].frequency = count;
                                break;
                            }
                        }
                    }
                    else
                    {
                        count++;
                    }
                }
                int min = 0;
                int temp_min = 0;
                //find which one should be deleted

                //find the smallest ARB
                for (int k = 0; k < memory_data.size() - 1; k++)
                {
                    if (memory_data[min].reg > memory_data[k].reg)
                    {
                        min = k;
                    }
                }
                temp_min = min;
                int count_min = 0;
                //search the times of the smallest ARB, if it greater than 1, find the smallest frequency
                for (int k = 0; k < memory_data.size() - 1; k++)
                {
                    if (memory_data[k].reg == memory_data[temp_min].reg)
                    {
                        count_min++;
                    }
                }

                if (count_min > 1)
                {
                    for (int k = 0; k < memory_data.size() - 1; k++)
                    {
                        if (memory_data[k].reg == memory_data[temp_min].reg && memory_data[min].frequency > memory_data[k].frequency)
                        {
                            min = k;
                        }
                    }
                }
                if (memory_data[min].status == "W")
                {
                    disk_write++;
                }
                memory_data.erase(memory_data.begin() + min);
                disk_read++;
            }
            else					// if the new in the memory vector, don't insert the new
            {
                memory_data[temp].reg.replace(0, 1, "1");
                if (memory_data[j].status == "R" && input_data[i].status != memory_data[j].status)
                {
                    memory_data[j].status = input_data[i].status;
                }
            }

            inter_count++;
            read = true;
        }
        for (int l = 0; l < memory_data.size(); l++)
        {
            memory_data[l].frequency = 0;
        }
    }
}

void print(vector <Trace> input_data)
{
    for (int i = 0; i < input_data.size(); i++)
    {
        cout << i + 1 << ": " << input_data[i].status << " page_num: " << input_data[i].page_num << endl;
        //cout << i << ": " << input_data[i].status << " memory: " << input_data[i].memory << " page_num: " << input_data[i].page_num << " frames: " << input_data[i].reg << endl;

    }
    cout << "------------------------" << endl;
}


int main(int argc, char* argv[])
{
    string str;
    freopen(argv[1], "r", stdin);
    pagesize = stoi(argv[2]);
    frame = stoi(argv[3]);
    str = argv[4];
    if (str == "FIFO")
    {
        input();
        FIFO();
    }
    else if (str == "LRU")
    {
        input();
        LRU();
    }
    else if (str == "ARB")
    {
        reg_num = stoi(argv[5]);
        interval = stoi(argv[6]);
        input();
        ARB();
    }
    else if (str == "WSARB-1")
    {
        reg_num = stoi(argv[5]);
        interval = stoi(argv[6]);
        win_size = stoi(argv[7]);
        input();
        WSARB_1();
    }
    else if (str == "WSARB-2")
    {
        reg_num = stoi(argv[5]);
        interval = stoi(argv[6]);
        win_size = stoi(argv[7]);
        input();
        WSARB_2();
    }
    cout << "events in trace: " << events_num << endl;
    cout << "total disk reads: " << disk_read << endl;
    cout << "total disk writes: " << disk_write << endl;
    cout << "page faults: " << disk_read << endl;

    return 0;
}


