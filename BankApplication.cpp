// Watler's codes

#include <fcntl.h>
#include <iostream>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

// user data
struct BankApplication 
{
    int accountNumber;
    int transitNumber;
    double balance;
};

using namespace std;

int main() 
{
    int retVal = 0;
    int selection;
    double amount;
    const int MEM_SIZE = 64;
    const char BankDatabase[]="BankDatabase.txt";
    struct stat sb;
    bool fileExisted=true;

    //if the file did not exist
    if( stat( BankDatabase, &sb ) == -1 ) 
    {
        fileExisted=false;
    }

    // file initialization
    int openFlags = O_RDWR | O_CREAT;
    mode_t filePerms =  S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;/* rw-rw-rw- */
    int fd = open( BankDatabase, openFlags, filePerms );

    //if the file did not exist, initialize its memory
    if(!fileExisted) 
    {
        cout<<"Zero the file's memory"<<endl;
        char buf[MEM_SIZE];
        memset(buf,0,MEM_SIZE);
        write(fd, buf, MEM_SIZE);
    }
    if(fd== -1) 
    {
        cout<<"ERROR: Cannot open "<<BankDatabase<<endl;
        cout<<strerror(errno)<<endl;
        retVal = -1;
    }





    // memory mapping begins
    BankApplication *addr;

    if(retVal==0) 
    {
        addr = (BankApplication *)mmap(NULL, MEM_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);

        if(addr == MAP_FAILED) 
        {
            cout<<"ERROR: mmap failed"<<endl;
            cout<<strerror(errno)<<endl;
            retVal = -3;
        }

        //We can close the file descriptor if mmap is successful.
        //If mmap is unsuccessful, we want to close the file descriptor anyway.
        close(fd);
    }





    // user menu
    if(retVal==0) 
    {
        do 
        {
            system("clear");
	        cout<<addr->accountNumber<<" "<<addr->transitNumber<<" balance $"<<addr->balance<<endl<<endl;
            cout<<"Make a selection:"<<endl;
            cout<<"1.Change Account Number."<<endl;
            cout<<"2.Change Transit Number."<<endl;
            cout<<"3.Withdraw."<<endl;
            cout<<"4.Deposit."<<endl;
            cout<<"5.Refresh."<<endl;
            cout<<"0.Quit."<<endl;
            cout<<endl;
            cout<<"Selection: ";
            cin>>selection;

            switch(selection) 
            {
                case 1:
                    cout<<"Enter the new account number: ";
                    cin>>addr->accountNumber;
                    msync(addr, sb.st_size, MS_SYNC);
                break;

                case 2:
                    cout<<"Enter the new transit number: ";
                    cin>>addr->transitNumber;
                    msync(addr, sb.st_size, MS_SYNC);
                break;

                case 3:
                    cout<<"Enter the withdrawal amount: $";
                    cin>>amount;
                    addr->balance -= amount;
                    msync(addr, sb.st_size, MS_SYNC);
                break;

                case 4:
                    cout<<"Enter the deposit amount: $";
                    cin>>amount;
                    addr->balance += amount;
                    msync(addr, sb.st_size, MS_SYNC);
                break;

                case 5:
                    //Refresh
                break;

                case 0:
                    cout<<"Goodbye!"<<endl;
                break;

                default:
                    cout<<"Invalid Selection!"<<endl;
                    sleep(3);
            }
        } 
        while (selection!=0);
    }





    if(retVal==0) 
    {
        munmap(addr, sb.st_size);
    }

    return retVal;
}