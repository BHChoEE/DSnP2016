#nclude <iostream>
#include <string>
#include "p2Table.h"
#include<iomanip>

using namespace std;

int main()
{
   Table table;

   // TODO: read in the csv file
   
   string csvFile;
   cout << "Please enter the file name: ";
   cin >> csvFile;
   if (table.read(csvFile))
      cout << "File \"" << csvFile << "\" was read in successfully." << endl;
   else exit(-1); // csvFile does not exist.

   // TODO read and execute commands
 //  table.PRINT();
    while (true) 
   {
    
            string command;
            cin>>command;
            if(command=="PRINT")
            {
                table.PRINT();
            }
            else if(command=="SUM")
            {
                int col;
                cin>>col;
                cout<<"The summation of data in column  #"<<col<<" is "<<table.SUM(col)<<"."<<endl;
            }
            else if(command=="AVE")
            {
                int col;
                cin>>col;
                cout<<"The average of data in column #"<<col<<" is "<<fixed<<setprecision(1)<<table.AVE(col)<<"."<<endl;

            }
            else if(command=="MAX")
            {
                int col;
                cin>>col;
                cout<<"The maximum of data in column #"<<col<<" is "<<table.MAX(col)<<"."<<endl;

            }
            else if (command =="MIN")
            {
                int col;
                cin>>col;
                cout<<"The minimum of data is column #"<<col<<" is "<<table.MIN(col)<<"."<<endl;
            }
            else if (command=="COUNT")
            {
                int col;
                cin>>col;
                cout<<"The distinct count of data in column #"<<col<<" is "<<table.COUNT(col)<<"."<<endl;

            }
            else if(command=="ADD")
            {
                string add;
                getline(cin,add);
                add=add.substr(1,add.length()-1);
                table.ADD(add);
            }
            else if(command=="EXIT")
                break;
   
   }
}

