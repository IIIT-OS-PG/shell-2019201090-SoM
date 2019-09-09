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
#include <pwd.h>
#include <termios.h>
#include <ctype.h>


#define history_size 200

using namespace std;

extern char **environ;
struct  termios orig_termios;
map< string, string > map_alias;
map< string, string > map_environment;
vector< string > history_vector;
string current_working_direc;

void displayTerminal();
void displayPrompt();
void environGet();
void environSet();
int exportEnvironment(char *new_value);
void disableRawMode();
void enableRawMode();
char * takeInputFromTerminal();
void displayHistory();
int removePipe(char *raw_input, char **processed_input);
void redirectionFunction(char **processed_input3, int m);
int redirectionDetect(char *processed_input1, char **processed_input3);
int removeSpace(char *processed_input1, char **processed_input2);
void executeCommand(char **processed_input);
void executeCommandWithPipe(char **processed_input, int no_of_pipes);
void changeDirectory(char *directory_path);
int checkAlias(char *processed_input);
string returnAlias(char *input_alias);



void displayTerminal()
{
	cout << "\033[H\033[J";
	cout << "MY SHELL ====>>> " << endl;
}

void displayPrompt()
{
	string token1 = "PS1";
	if(geteuid() == 0)
	{
		string ps_value = map_environment[token1];
		int l1 = ps_value.length()-1;
		string ps_value1 = ps_value.substr(0,l1-1);
		ps_value1 +="#";
		map_environment[token1] = ps_value1;
		environSet();
	}
	//string token = "PS1";
	string ps_value = map_environment[token];
	if(ps_value.length() == 0)
		cout << "$" ;
	else 
	{
		int l = ps_value.length()-1;
		char ch = ps_value[l];
		if(ch == '$')
			cout << ps_value;
		else
			cout << ps_value << "$";
	}
}

void environGet()
{

	uid_t uid = geteuid(); 
	struct passwd *pw = getpwuid(uid); 
	const char *homedir; 
	const char *logged_user;
	if((homedir = getenv("HOME")) == NULL)
		homedir = pw->pw_dir;
	logged_user = pw->pw_name;
    char *path_variable = getenv("PATH");
	char hostname_variable[1024];
	hostname_variable[1023] = '\0';
	gethostname(hostname_variable, 1023);
	char current_path[1024];
	current_path[1023] = '\0';
	getcwd(current_path, 1023);
	string temp(current_path);
	current_working_direc = temp;

	string str_path_variable(path_variable);
	string str_home_variable(homedir);
	string str_user_variable(logged_user);
	string str_hostname_variable(hostname_variable);

	map_environment.insert({"PATH" , str_path_variable});
	map_environment.insert({"HOME" , str_home_variable});
	map_environment.insert({"USER" , str_user_variable});
	map_environment.insert({"HOSTNAME" , str_hostname_variable});
	map_environment.insert({"PS1" , "MYSHELL$"});
	map_environment.insert({"DBUS_SESSION_BUS_ADDRESS" , "unix:path=/run/user/1000/bus"});
	map_environment.insert({"DISPLAY" , ":0"});
	//map_environment.insert({"XDG_RUNTIME_DIR" , "/run/user/1000"});
	//map_environment.insert({"XDG_SESSION_TYPE", "x11"});

}

void environSet()
{
	string filename = current_working_direc + "/myshell_bashrc";
	ofstream file;
    file.open(filename);
    if(file.is_open())
    {
    	for(auto x: map_environment)
    		file << x.first + "=" + x.second << endl;
    }
    file.close();

    /*
    //char *environ_array[7];
    //string line1 = "PATH=" + map_environment["PATH"];
    string line2 = "HOME=" + map_environment["HOME"];
    string line3 = "USER=" + map_environment["USER"];
    string line4 = "HOSTNAME=" + map_environment["HOSTNAME"];
    string line5 = "DBUS_SESSION_BUS_ADDRESS=" + map_environment["DBUS_SESSION_BUS_ADDRESS"];
    string line6 = "DISPLAY=" + map_environment["DISPLAY"];
    
    //cout << (char *)line1.c_str() << endl;

    /*environ_array[0] = (char *)line1.c_str();
    environ_array[1] = (char *)line2.c_str();
    environ_array[2] = (char *)line3.c_str();
    environ_array[3] = (char *)line4.c_str();
    environ_array[4] = (char *)line5.c_str();
    environ_array[5] = (char *)line6.c_str();
    environ_array[6] = NULL;


    char *environ_array[]={(char *)("PATH=" + map_environment["PATH"]).c_str(), 
    						(char *)line2.c_str(), 
    						(char *)line3.c_str(), 
    						(char *)line4.c_str(), 
    						(char *)line5.c_str(), 
    						(char *)"DISPLAY=:0", NULL};
    
    memcpy(environ, environ_array, sizeof(environ_array));

    //environ = environ_array;
   	
    */

    string token1 = "PATH=";
    string token2 = "HOME=";
    string token3 = "USER=";
	string token4 = "HOSTNAME="; 
	string token5 = "DBUS_SESSION_BUS_ADDRESS=";
	string token6 = "DISPLAY=";

    char *environ_array[1000];
    ifstream file2;
    file2.open(filename);
    string line1,line2,line3,line4,line5,line6;
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
           	if(line.find(token5) != string::npos)
           		line5 = line;
           	if(line.find(token6) != string::npos)
           		line6 = line;

        }  
        file2.close();      
    }
    environ_array[0] = (char *)line1.c_str();
    environ_array[1] = (char *)line2.c_str();
    environ_array[2] = (char *)line3.c_str();
    environ_array[3] = (char *)line4.c_str();
    environ_array[4] = (char *)line5.c_str();
    environ_array[5] = (char *)line6.c_str();
    environ_array[6] = NULL;
    
    //environ = environ_array;

    memcpy(environ, environ_array, sizeof(environ_array));						 


    /*for (char **env = environ; *env != 0; env++)
  	{
    	char *thisEnv = *env;
    	cout << thisEnv << endl; 
  	}*/
}

int exportEnvironment(char *processed_input)
{
	string str1(processed_input);
	string temp_export = str1;
	char *export_env = (char *)temp_export.c_str();
	char *processed_env[100];
	char *token = strtok(export_env, "=");
	int i = 0;
	while(token != NULL)
	{
		processed_env[i++] = token;
		token = strtok(NULL, "=");
	}
	processed_env[i] = NULL;
	if(i > 1)
	{
		string str1 = processed_env[0];
		int len1 = str1.length();
		string strenv = str1.substr(0,6);
		if(strenv == "export")
		{
			string str2 = str1.substr(7,len1-7);
			string str3 = processed_env[1];
			map< string, string >::iterator itr;
			itr = map_environment.find(str2);
			if(itr != map_environment.end())
			{
				map_environment[str2] = str3;
				environSet();
			}
			else
				cout << "Environment variable not found !!!" << endl;
   			return 1;
		}			
	}
	return 0;
}

void disableRawMode()
{
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void enableRawMode()
{
	tcgetattr(STDIN_FILENO, &orig_termios);
	//atexit(disableRawMode);
	struct termios raw = orig_termios;
	raw.c_lflag &= ~(ECHO | ICANON);
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

char * takeInputFromTerminal()
{
	/*char input[1000]={};
	//enableRawMode();
	char c;
	int pos = 0;
	while(true)
	{
		c=getchar();
		if (c == 127) 
		{
			if(pos > 0)
            {
            	printf("\b");
            	printf(" ");
            	printf("\b");
            
            	input[pos] = '\0';
            	pos--;
            }
            continue;
        }
		else if(c == 10)
		{
			cout << endl;
			break;
		}
		else
		{
			input[pos]=c;
			pos++;
			putchar(c);
		}
	}
	input[pos]='\0';
	string str="";
	if(pos != 0)
		str = input;
	*/
	string str; 
	getline (cin, str);
	if(str != "")
	{
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
	else
		return NULL;
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
	string temp = returnAlias(processed_input3[0]);
	if(temp != "")
		processed_input3[0] = (char *)temp.c_str();
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
		int z;
		if(m == 1)
			z = open(*command2, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if(m == 2)
			z = open(*command2, O_APPEND | O_WRONLY | O_CREAT, 0644);
		dup2(z, 1);
		if(execvp(command1[0], command1) == -1)
			cout << "Command not executed !!!" << endl;
		exit(0);
		close(z);
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
			if(processed_input[1] == NULL)
			{
				char *change_dir = (char *)map_environment["HOME"].c_str();
				int change_dir1 = chdir(change_dir);
				if(change_dir1 == -1)
					cout << "Error during change directory !!!" << endl;
			}
			else
				changeDirectory(processed_input[1]);
		}
		else if(strcmp(processed_input[0], "history") == 0)
		{
			displayHistory();
		}
		else
		{
			string temp = returnAlias(processed_input[0]);
			//cout << processed_input[0] << endl;
			//string temp1(processed_input[1]);
			//cout << temp1 << endl;
			//string temp2 = temp  temp + " " + temp1;
			//cout << temp1 <<endl;
			//if(processed_input[1] != NULL)
			//	temp = temp  + temp1; 
			//cout << "2"<< endl;
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
				{
					cout << "Command not executed !!!" << endl;
					exit(0);
				}
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
        if(direc_path.length() == 1)
        	final_path = map_environment["HOME"];
        else
         	final_path = map_environment["HOME"] + direc_path.substr(1, direc_path.length()-1);
    }
	else
		final_path = direc_path;
	final_directory_path = (char *)final_path.c_str();
	int change_dir = chdir(final_directory_path);
	if(change_dir == -1)
		cout << "Error during change directory !!!" << endl;
	
}

int checkAlias(char *processed_input)
{
	string temp_alias(processed_input);
	string temp_alias1 = temp_alias;
	char *alias_input = (char *)temp_alias1.c_str();
	char *processed_alias[100];
	char *token = strtok(alias_input, "=");
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
	char *environ_variables[100]; 
    displayTerminal();
    environGet();
    environSet();
    //enableRawMode();
	while(true)
	{
		displayPrompt();
		char *input_from_terminal;
		char *processed_input[1000];
		input_from_terminal = takeInputFromTerminal();
		if(input_from_terminal == NULL)
			continue;
		int no_of_pipes = 0;
		if(strcmp(input_from_terminal, "exit") == 0)
			break;
		else
		{
			if((checkAlias(input_from_terminal)) == 0)
			{	
				if((exportEnvironment(input_from_terminal)) == 0)
				{
					no_of_pipes = removePipe(input_from_terminal, processed_input);
					if(no_of_pipes > 0)
						executeCommandWithPipe(processed_input, no_of_pipes);
					else
					{
						char *processed_input2[1000];
						if((removeSpace(processed_input[0], processed_input2)) == 0)
						{
							if((strcmp(processed_input2[0], "cd")) == 0)
							{
								if(processed_input2[1] == NULL)
								{
									char *change_dir = (char *)map_environment["HOME"].c_str();
									int change_dir1 = chdir(change_dir);
									if(change_dir1 == -1)
										cout << "Error during change directory !!!" << endl;
								}
								else
									changeDirectory(processed_input2[1]);
							}
							else if((strcmp(processed_input2[0], "open") == 0))
							{
								string str_media(processed_input2[1]);
								int l= str_media.length();
								string str1 = str_media.substr(l-4,l-1);
								pid_t pid = fork();
								if( pid == 0)
								{
									if(str1.compare(".mp4") == 0)
									{
										processed_input2[0] = (char *)"vlc";

										if(execvp(processed_input2[0], processed_input2) == -1)
										{
											cout << "Command not executed !!!" << endl;
											//exit(0);
										}
									}
									else
									{
										processed_input2[0] = (char *)"gedit";
										if(execvp(processed_input2[0], processed_input2) == -1)
										{
											cout << "Command not executed !!!" << endl;
											//exit(0);
										}
									}
									exit(0);
								}
								else
								{
									wait(NULL);
								}	
							}
							else
								executeCommand(processed_input2);
						}
					}
				}
				else
					environSet();
			}
		}	
	}
	//disableRawMode();
	//exit(0);
	return 0;	
}