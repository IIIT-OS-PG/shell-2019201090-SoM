#include <bits/stdc++.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream> 
#include <fstream>
#include <string> 
#include <map>

#define history_size 200

using namespace std;

extern char **environ;

void displayTerminal();
void environSet();
char * takeInputFromTerminal();
int removeSpace(char *processed_input1, char **processed_input2);
int removePipe(char *raw_input, char **processed_input);
void executeCommand(char **processed_input);
void changeDirectory(char *directory_path);
int checkAlias(char *processed_input);
string returnAlias(char *input_alias);
void executeCommandWithPipe(char **processed_input, int no_of_pipes);



map< string, string > map_alias;
vector< string > history_vector;


void displayTerminal()
{
	cout << "\033[H\033[J";
	cout << "MY SHELL ====>>> " << endl;
}

void environSet()
{
    char *path_variable = getenv("PATH");
	char *home_variable = getenv("HOME");
	char *user_variable = getenv("USER");
	char hostname_variable[1024];
	hostname_variable[1023] = '\0';
	gethostname(hostname_variable, 1023);

	string str_path_variable(path_variable);
	string str_home_variable(home_variable);
	string str_user_variable(user_variable);
	string str_hostname_variable(hostname_variable);

	string str_file_path = "myshell_bashrc";

	char *filename = (char *)str_file_path.c_str();

    ofstream file1;
    file1.open(filename);
    if(file1.is_open())
    {
    	file1 << "PATH=" + str_path_variable << endl;
    	file1 << "HOME=" + str_home_variable << endl;
    	file1 << "USER=" + str_user_variable << endl;
    	file1 << "HOSTNAME=" + str_hostname_variable << endl;

    }
    file1.close();


    string token1 = "PATH=";
    string token2 = "HOME=";
    string token3 = "USER=";
	string token4 = "HOSTNAME=";    

	char *environ_array[1000];
    ifstream file2;
    file2.open(str_file_path);
    string line1,line2,line3,line4;
    if(file2.is_open())
    {
    	string line;
        while (getline(file2, line))
        {
        	if(line.find(token1) != string::npos)
           		line1 = line;           		
            if(line.find(token2) != string::npos)
           		line2 = line;           		
            if(line.find(token3) != string::npos)
           		line3 = line;           		
            if(line.find(token4) != string::npos)
           		line4 = line;           		
        }  
        file2.close();      
    }
    environ_array[0] = (char *)line1.c_str();
    environ_array[1] = (char *)line2.c_str();
    environ_array[2] = (char *)line3.c_str();
    environ_array[3] = (char *)line4.c_str();
    
    environ = environ_array;

    /*for (char **env = environ; *env != 0; env++)
  	{
    	char *thisEnv = *env;
    	cout << thisEnv << endl; 
  	}
	*/
}

char * takeInputFromTerminal()
{
	string str; 
	getline (cin, str);
	if(history_vector.size() < history_size)
	{
		history_vector.push_back(str);
	}
	else
	{
		vector< string >::iterator hv1;
		hv1 = history_vector.begin();
		history_vector.erase(hv1);
		history_vector.push_back(str);
	}
	char *read_input = new char[str.length() + 1];
	strcpy(read_input, str.c_str());
	return read_input;
}

void displayHistory()
{
	for(int i=0; i<history_vector.size(); i++)
		cout << history_vector[i] << endl;
}


int removePipe(char *raw_input, char **processed_input)
{
	char *token = strtok(raw_input, "|");
	int i = 0, no_of_pipes = 0;
	while(token != NULL)
	{
		processed_input[i++] = token;
		token = strtok(NULL, "|");
	}
	no_of_pipes = i-1;
	return no_of_pipes;
}

void redirectionFunction(char **processed_input3, int m)
{
	int res = 0;
	char *command1[100], *command2[100];
	int x = removeSpace(processed_input3[0] , command1);
	int y = removeSpace(processed_input3[1] , command2);
	pid_t pid;
	int p[2];
	pipe(p);
	pid = fork();
	if(pid < 0)
		perror("Fork Failed !");
	else if(pid == 0)	
	{
		close(p[1]);
		int x;
		if(m == 1)
		{
			x = open(*command2, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		}
		if(m == 2)
		{
			x = open(*command2, O_APPEND | O_WRONLY | O_CREAT, 0644);
		}
		dup2(x, 1);
		if(execvp(command1[0], command1) == -1)
			cout << "Command not executed !!!" << endl;
		exit(0);
		close(x);
	}
	else
	{
		wait(NULL);	
	}
}

int redirectionDetect(char *processed_input1, char **processed_input3)
{
	char *token;
	int i = 0, no_of_redic = 0;
	while( (token = strsep(&processed_input1,">")) != NULL )
	{
        processed_input3[i++] = token;
	}
	no_of_redic = i-1;
	if(no_of_redic == 2)
	{
		processed_input3[1] = processed_input3[2];
		processed_input3[2] = NULL;

	}
	return no_of_redic;
}

int removeSpace(char *processed_input1, char **processed_input2)
{
	int redirect1 = 0;
	char *processed_input3[100];
	redirect1 = redirectionDetect(processed_input1, processed_input3);
	if(redirect1 != 0)
	{
		redirectionFunction(processed_input3, redirect1);
		return redirect1;
	}
	else
	{
		char *token = strtok(processed_input1, " ");
		int i = 0;
		while(token != NULL)
		{
			processed_input2[i++] = token;
			token = strtok(NULL, " ");
		}
		processed_input2[i] = NULL;
		return 0;
	}
}


void executeCommand(char **processed_input)
{
	pid_t pid;
	pid = fork();

	if(pid < 0)
		perror("Fork Failed !");
	else if(pid == 0)
	{
		if(strcmp(processed_input[0], "cd") == 0)
		{
			changeDirectory(processed_input[1]);
		}
		else if(strcmp(processed_input[0], "history") == 0)
		{
			displayHistory();
		}
		else
		{
			string temp = returnAlias(processed_input[0]);
			char *processed_alias1[1000];
			char *processed_alias2[1000];
			if(temp != "")
			{
				char *read_input = new char[temp.length() + 1];
				strcpy(read_input, temp.c_str());
				int no_of_pipes1 = removePipe(read_input, processed_alias1);
				if(no_of_pipes1 > 0)
				{
					executeCommandWithPipe(processed_alias1, no_of_pipes1);
				}
				else
				{	
					if((removeSpace(read_input, processed_alias2)) == 0)
					{
						if(execvp(processed_alias2[0], processed_alias2) == -1)
							cout << "Command not executed !!!" << endl;
					}
				} 	
			}
			else
			{
				if(execvp(processed_input[0], processed_input) == -1)
					cout << "Command not executed !!!" << endl;
			}		
		}
	}
	else
		wait(NULL);
}

void executeCommandWithPipe(char **processed_input, int no_of_pipes)
{
	int p[2];
	pid_t pid;
	int repeat = 0;
	while(*processed_input != NULL)
	{
		pipe(p);
		pid = fork();
		if(pid < 0)
			perror("Fork Failed !!!");
		else if(pid == 0)
		{
			dup2(repeat, 0);
			if(*(processed_input + 1) != NULL)
				dup2(p[1], 1);
			close(p[0]);
			close(p[1]);
			char *processed_input2[1000];
			if((removeSpace(processed_input[0] , processed_input2)) == 0)
				executeCommand(processed_input2);
			exit(0);
		}
		else
		{
			wait(NULL);
			close(p[1]);
			repeat = p[0];
			processed_input++;
		}
	}
}

void changeDirectory(char *directory_path)
{
	char *final_directory_path;
	string final_path = "";
	string direc_path(directory_path); 
	if(directory_path[0] == '~')
	{
		string str_file = "myshell_bashrc";	
		string token = "HOME=";
		ifstream file;
    	file.open(str_file);
    	string line1;
    	if(file.is_open())
    	{
    		string line;
        	while (getline(file, line))
        	{
        		if(line.find(token) != string::npos)
           			line1 = line;           		
           	}		           		
        }
        file.close();
        string sub_home = line1.substr(5, line1.length()-4);   
        if(direc_path.length() == 1)
        {
        	final_path = sub_home;
        }
        else
        {
        	string direc_path1 = direc_path.substr(1, direc_path.length());
         	final_path = sub_home + direc_path1;
        }
    }
	else
	{
		final_path = direc_path;
	}
	final_directory_path = (char *)final_path.c_str();
	int change_dir = chdir(final_directory_path);
	if(change_dir == -1)
		cout << "Error during change directory !!!" << endl;
}

int checkAlias(char *processed_input)
{
	char *processed_alias[100];
	char *token = strtok(processed_input, "=");
	int i = 0;
	while(token != NULL)
	{
		processed_alias[i++] = token;
		token = strtok(NULL, "=");
	}
	processed_alias[i] = NULL;
	if(i > 1)
	{
		map<string, string>::iterator it1;
		string str1 = processed_alias[0];
		int len1 = str1.length();
		string stralias = str1.substr(0,5);
		if(stralias == "alias")
		{
			string str4 = str1.substr(6,len1-6);
			string str2 = processed_alias[1];
			int len2 = str2.length();
			string str5 = str2.substr(1,len2-2);
			it1 = map_alias.find(str4);
			if (it1 != map_alias.end())
	  		{	
	  			map_alias[str4] = str5;
	  		}
			map_alias.insert({str4, str5});
			return 1;
		}	
	}
	return 0;
}

string returnAlias(char *input_alias)
{
	map<string, string>::iterator it;
	string str1(input_alias);
	it = map_alias.find(str1);
  	if (it != map_alias.end())
  	{
  		string str = it->second;
  		return str;
  	}
  	return "";
}

int main()
{
	char *environ_variables[1000]; 
    displayTerminal();
    environSet();

	while(true)
	{
		cout << "$";
		char *input_from_terminal;
		char *processed_input[1000];
		input_from_terminal = takeInputFromTerminal();
		int no_of_pipes = 0;
		if((checkAlias(input_from_terminal)) == 0)
		{	
			no_of_pipes = removePipe(input_from_terminal, processed_input);
			if(no_of_pipes > 0)
			{
				executeCommandWithPipe(processed_input, no_of_pipes);
			}
			else
			{
				char *processed_input2[1000];
				if((removeSpace(processed_input[0], processed_input2)) == 0)
				{
					executeCommand(processed_input2);
				}
			}
		}		
	}
	return 0;	
}