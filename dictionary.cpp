#include<iostream>
#include<fstream>
#include<sstream>
#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<string>
#include<map>
#include<set>
#include<algorithm>
#include<limits>
using  namespace std;

#if defined linux || defined __linux__ || defined __linux  // Linux
#define PLATFORM_CONFIG_PATH    getenv("HOME")+string("/.dict_config.txt")
#define PLATFORM_RESET_COMMAND  "reset"

#elif defined __WIN32 || defined __WIN64  // Windows
#define PLATFORM_CONFIG_PATH    getenv("USERPROFILE")+string("\\My Documents\\dict_config.txt")
#define PLATFORM_RESET_COMMAND  "cls"

#else
#error "Untested platform"
#endif

/**********************************************************************************************
 * Global Declarations begin */

const string config_path=PLATFORM_CONFIG_PATH;  // Path to store configuration File which will contain location of dictionary and its backup
const string clear_screen=PLATFORM_RESET_COMMAND;  // OS-Specific "clear-screen" command

map<string,string> dictionary,dictionary_bak;  // The underlying Data Structure for dictionary
string dict_path;   // dict_path stores the location of the dictionary
string bak_dict_path;   // bak_dict_path stores the location of the backup dictionary

/***********************************************************************************************
 * Global Declarations end */


// Check if the address "path" exists and is readable
bool isFileReadable(const string& path)
{
    ifstream fin(path);
    return fin.good();
}


// Check if the address "path" exists and is writable
bool isFileWritable(const string& path,const bool& file_already_present)
{
    ofstream fout(path);  // Try to create a dummy file with write permissions
    if(fout.good())  // Dummy file is created successfully in write mode
    {
        if(!file_already_present)
        {
            fout.close();  // Close the dummy file so that it can be removed
            if(remove(path.c_str())) // Remove the dummy file
            {
                cerr<<"Could not remove "<<path<<endl;
                return false;  // Unable to remove file successfully
            }
        }
        return true;  // Everything OK!
    }
    return false;  // Something gone wrong
}


// Input locations of dictionary and its backup from the user
void getLocations()
{
    cout<<"\nEnter the path to store dictionary\n";
    getline(cin,dict_path);

    cout<<"\nEnter the path to store backup of dictionary\n";
    getline(cin,bak_dict_path);
}


/* Check if the configuration file is present or not.
 * If it is present, then read the locations of dictionary and its backup,
 * else, create it.
 */
void checkConfig()
{
    if(!isFileReadable(config_path))  // Config file does not exist or is not readable
    {
        // Try to make a fresh config file
        if(isFileWritable(config_path,false))
        {
            ofstream fout(config_path);   // Create a new configuration file
            fout<<"\n\t\t\tDo not modify this file!!!!\n\n#";  // Write warning message
            getLocations();   // Get the locations of dictionary and its backup
            fout<<dict_path<<"\n"<<bak_dict_path<<"\n";  // Write the locations to the config file
        }
        else  // Can not write to the location "config_path"
        {
            throw "Creating \""+config_path+"\"";
        }
    }

    ifstream fin(config_path);
    fin.ignore(numeric_limits<streamsize>::max(),'#');  // Eat the warning message
    getline(fin,dict_path);  // Input the location of the dictionary from the config file into "dict_path"
    getline(fin,bak_dict_path);  //   Input the location of the backup dictionary from the config file into "bak_dict_path"
}


// Get the contents of the dictionary from "path" and store it in "mapping"
void readFromFile(const string& path,map<string,string> &mapping)
{
    if(!isFileReadable(path))  //  File is not present at "path"
    {
        // Try to make a new file
        if(isFileWritable(path,false))
        {
            ofstream fout(path);  // Create a blank file
        }
        else
        {
            throw "Creating \""+path+"\"";
        }
    }

    mapping.clear();
    string word,meaning;
    ifstream fin(path);
    while(getline(fin, word))
    {
        if(word.empty())
            continue;
        if (getline(fin,meaning,'$') && (!meaning.empty()))   // A meaning is terminated by a '$'
        {
            mapping.emplace(make_pair(word, meaning));
        }
        fin.get(); // Take the remaining '\n'
    }

}

// Save the dictionary to the file..
void writeToFile(const string& path,map<string,string> &mapping)
{
    if(!isFileWritable(path,true))
    {
        throw "Writing to \""+path+"\"";
    }

    ofstream fout(path);
    for(const auto& it:mapping)
    {
        fout<<it.first<<"\n"<<it.second<<"$\n";
        // Word terminated by a '\n'..
        // Meaning terminated by a "$\n"..
    }
}

void show(const map<string,string> &mapping=dictionary,int at_a_time=0)
{
    // Show the contents of "mapping" with "at_a_time" word-meaning pairs displayed at a time...
    int i=1;
    cout<<"\t\t\t"<<"No. of Entries = "<<mapping.size()<<"\n\n";
    if(at_a_time==0) // Show all the contents at once..
        at_a_time=mapping.size();
    for(const auto& it:mapping)
    {
        cout<<i<<"). "<<it.first<<"\n\n"<<it.second<<"\n";
        if( (i%at_a_time == 0) && i!=(int)mapping.size() )
        {
            cout<<"\t\tPress <Enter> to resume..   ";
            cin.get();  // Wait for the user...
        }
        ++i;
    }
}

// Retrieve the part of "s" before a " " is encountered..
string beforeSpace(const string &query)
{
    stringstream ss(query);
    string ans;
    ss>>ans;
    return ans;
}

// Retrieve the part of "s" after a " " is encountered..
string afterSpace(const string &query)
{
    stringstream ss(query);
    string ans;
    ss>>ans>>ans;
    return ans;
}

// Check if the query recieved is a keyword ..
bool isReserved(string query)
{
    transform(query.begin(),query.end(),query.begin(),::tolower);
    static set<string> reserved= {"help","clear","rebuild","show","bak","showbak","loadbak","test","refresh","remove"};
    return reserved.find(beforeSpace(query))!=reserved.end();
}

// Show the help menu..
void showHelp()
{
    static map<string,string> help;
    help["\n\"clear\""]="Clear screen...\n";
    help["\n\"rebuild\""]="Delete all the present entries of the dictionary...\n";
    help["\n\"show [no. of entries at a time]\""]="Show all the present entries of the dictionary...\n";
    help["\n\"bak\""]="Backup all the present entries of the dictionary...\n";
    help["\n\"showbak [no. of entries at a time]\""]="Show all the present entries of the Backup dictionary...\n";
    help["\n\"loadbak\""]="Replace the current dictionary with the most recent backup...\n";
    help["\n\"test\""]="Take a self-test by showing any randomly chosen word from the dictionary and making you guess its meaning...\n";
    help["\n\"refresh\""]="Reload the dictionary from the file...\n";
    help["\n\"remove <word>\""]="Remove a word...\n";
    for(const auto& it:help)
    {
        cout<<it.first<<" -- "<<it.second;
    }
}

// Empty the dictionary..
void rebuild()
{
    dictionary.clear();
    cout<<"Dictionary cleared!!\n\n";
}

// Remove a word-meaning pair from the dictionary..
void removeWord(const string& word)
{
    decltype(dictionary.begin()) it;
    if((it=dictionary.find(word))!=dictionary.end())
    {
        dictionary.erase(it);
        cout<<word<<" erased!!\n";
    }
    else
    {
        cout<<"Word not found!!\n\n";
    }
}

// Show a random word-meaning pair from the dictionary..
void showRand()
{
    if(!dictionary.size()) // Dictionary is empty..
        return;
    decltype(dictionary.begin()) it=dictionary.begin();
    advance(it,rand()%dictionary.size());
    cout<<"\t\t\t\""<<it->first<<"\"\n\t\tPress <Enter> to show meaning..  ";
    cin.get();  // Wait for an "enter"...
    cout<<"\n\n"<<it->second<<"\n";
}

// Run the query..
void process(const string &query)
{
    string before_space=beforeSpace(query);
    string after_space=afterSpace(query);
    transform(before_space.begin(),before_space.end(),before_space.begin(),::tolower);
    if(before_space=="help")
    {
        showHelp();
    }
    else if(before_space=="rebuild")
    {
        rebuild();
    }
    else if(before_space=="show")
    {
        show(dictionary,atoi(after_space.c_str()));
    }
    else if(before_space=="clear")
    {
        system(clear_screen.c_str());
        // The compiler keeps warning the value returned here is unused, but I do not know how to use it !!!
    }
    else if(before_space=="bak")
    {
        writeToFile(bak_dict_path,dictionary);
    }
    else if(before_space=="showbak")
    {
        readFromFile(bak_dict_path,dictionary_bak);
        show(dictionary_bak,atoi(after_space.c_str()));
    }
    else if(before_space=="loadbak")
    {
        readFromFile(bak_dict_path,dictionary);
        writeToFile(dict_path,dictionary);
    }
    else if(before_space=="test")
    {
        showRand();
    }
    else if(before_space=="remove")
    {
        removeWord(after_space);
    }
    else if(before_space=="refresh")
    {
        readFromFile(dict_path,dictionary);
    }
}

// Input a Word/Query..
string getQuery()
{
    string query;
    cout<<"\nEnter query..  ";
    getline(cin,query,'\n');
    return query;
}

// Input the meaning of a word..
string getMeaning()
{
    string meaning;
    cout<<"\nEnter meaning..  ";
    char c;
    while(1)
    {
        c=cin.get();
        if(meaning.size()&&meaning[meaning.size()-1]=='\n'&&c=='\n')
            break;
        meaning+=c;
    }
    return meaning;
}

// Add a word to the dictionary..
void addWord(const string &word)
{
    decltype(dictionary.begin()) it;
    if((it=dictionary.find(word))!=dictionary.end())
    {
        cout<<"'"<<it->first<<"' already exists!!...\n\n"<<it->second<<"\n\nWant to overwrite?? (y/n)  ";
        char option;
        cin>>option;
        cin.ignore(numeric_limits<streamsize>::max(),'\n');
        if(option=='y'||option=='Y')
        {
            string temp=getMeaning();
            if(temp!="\n")
                it->second=temp;
        }
    }
    else
    {
        string temp=getMeaning();
        if(temp!="\n")
            dictionary[word]=temp;
    }
}


// Main starts from here...
int main()
{
    srand(time(NULL)); // For showRand()...
    try
    {
        checkConfig();
        readFromFile(dict_path,dictionary);
        while(1)
        {
            string query=getQuery();
            if(query.empty()) // Exit if the query entered is blank...
                break;
            else if(isReserved(query))
                process(query);
            else
                addWord(query);
        }
        writeToFile(dict_path,dictionary);
    }
    catch(const string& error_msg)
    {
        cerr<<"Fatal error while "<<error_msg<<" - "<<strerror(errno)<<"\n";
        cout<<"In case this error message persists, try deleting \""<<config_path<<"\" and resarting the programn\n\n";
        cerr<<"Exiting\n"<<endl;
        return -1;
    }

}
