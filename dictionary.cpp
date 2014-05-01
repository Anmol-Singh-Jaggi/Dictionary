#include<bits/stdc++.h>
using  namespace std;
#ifdef DEBUG
#include<Debug.h>
#endif

//*********************************************************************************************
// Global Declarations begin...

#if defined linux || defined __linux__ || defined __linux  // Linux
const char* config_path=strcat(getenv("HOME"),"/dict_config.txt");  // Path to store configuration File which will contain location of dictionary and its backup on Linux...
const char* clear_screen="reset";
#elif defined __WIN32 || defined __WIN64  // Windows
const char* config_path=strcat(getenv("USERPROFILE"),"\\My Documents\\dict_config.txt");  // Path to store configuration File which will contain location of dictionary and its backup on Windows...
const char* clear_screen="cls";
#endif

map<string,string> dictionary,dictionary_bak;  // The underlying Data Structure for dictionary...
char dict_path[1001];   // dict_path stores the location of the dictionary..
char bak_dict_path[1001];   // bak_dict_path stores the location of the backup dictionary..

// Global Declarations end...
//**********************************************************************************************

bool locationExists(const char* path)   // Check if the file stored at "path" has a valid location.. [ So that a dictionary can be created there ]
{
    ifstream fin(path);
    if(fin.good())  // A File is already there... so location is OK..
    {
        return true;
    }
    fin.close();
    ofstream fout(path);  // Try to create a file...
    if(fout.good())  // File is writable...
    {
        fout.close();
        if(remove(path)) // Remove the file which was created...
        {
            perror("Fatal Error ");
            cout<<"Unable to remove file - \""<<path<<"\"\nExiting\n"; // Permission Errors most probably...
            exit(1);
        }
        return true;
    }
    return false;
}

bool fileExists(const char* path)   // Check if a file at a location named "path" exists..
{
    ifstream fin(path);
    return fin.good();
}

void getLocations()  // Input locations of dictionary and its backup from the user...
{
    while(1)
    {
        cout<<"\nEnter the path to store dictionary\n[ For example - /home/anmol/Documents/dictionary.txt ]\n";
        cin.getline(dict_path,1000);
        if(!locationExists(dict_path))
        {
            perror("Error ");
            cout<<"Unable to create file at - \""<<dict_path<<"\"\nPlease Enter an alternate location\n";
        }
        else
        {
            break;
        }
    }
    while(1)
    {
        cout<<"\nEnter the path to store backup of dictionary\n[ For example - /home/anmol/Documents/dictionary_bak.txt ]\n";
        cin.getline(bak_dict_path,1000);
        if(!locationExists(bak_dict_path))
        {
            perror("Error ");
            cout<<"Unable to create file at - \""<<bak_dict_path<<"\"\nPlease Enter an alternate location\n";
        }
        else
        {
            break;
        }
    }
}

void checkConfig()  // Check if the dictionary file is present and read it if possible...
{
    if(!locationExists(config_path))  // Shouldn't happen frequently...
    {
        perror("Fatal Error ");
        cout<<"Unable to create configuration file at - \""<<config_path<<"\"\nExiting\n";
        exit(1);
    }
    if(!fileExists(config_path))
    {
        ofstream fout(config_path);   // Create a new configuration file...
        if(fout.good())
        {
            fout<<"\n\t\t\tDo not modify this file!!!!\n\n#";  // Write warning message...
            getLocations();   // Get the locations of dictionary and its backup...
            fout<<dict_path<<"\n"<<bak_dict_path<<"\n";  // Write the locations to the config file...
            fout.close();
        }
        else  // Shouldn't happen frequently...
        {
            perror("Fatal Error ");
            cout<<"Unable to create configuration file at - \""<<config_path<<"\"\nExiting\n";
            exit(1);
        }
    }
    ifstream fin(config_path);
    fin.ignore (std::numeric_limits<std::streamsize>::max(),'#');  // Eat the warning message...
    fin.getline(dict_path,1000);  // Input the location of the dictionary from the config file into "dict_path"...
    if(!locationExists(dict_path))
    {
        perror("Fatal Error ");
        cout<<"The path for dictionary - \""<<dict_path<<"\" is invalid\nTry restarting the program\n";
        remove(config_path);
        exit(1);
    }
    fin.getline(bak_dict_path,1000);  //   Input the location of the backup dictionary from the config file into "bak_dict_path"...
    if(!locationExists(bak_dict_path))
    {
        perror("Fatal Error ");
        cout<<"The path for dictionary backup- \""<<bak_dict_path<<"\" is invalid\nTry restarting the program\n";
        remove(config_path);
        exit(1);
    }
    fin.close();
}

void readFromFile(const char* path,map<string,string> &mapping)  // Get the contents of the dictionary from "path" and store it in memory as a mapping..
{
    if(!fileExists(path))
    {
        cout<<"Creating a new file at - "<<path<<"\n";
        ofstream fout(path);  // Create a blank file at "path"..
        if(!fout.good())
        {
            perror("Fatal Error ");
            cout<<"Unable to create a new file - \""<<path<<"\"\nExiting\n";
            exit(1);
        }
        fout.close();
    }
    mapping.clear();
    string word,meaning;
    ifstream fin(path);
    while(1)
    {
        getline(fin,word,'\n'); // A word is terminated by a '\n'..
        if(fin.eof())  // End-Of-File reached..
            break;
        if(word.empty())
            continue;
        getline(fin,meaning,'$');  // A meaning is terminated by a '$'...
        if(meaning.empty())
            continue;
        fin.get(); // Take the remaining '\n'..
        mapping[word]=meaning;
    }
    fin.close();
}

void writeToFile(const char* path,map<string,string> &mapping) // Save the dictionary to the file..
{
    ofstream fout;
    fout.open(path);
    if(!fout.good())
    {
        perror("Fatal Error ");
        cout<<"Unable to create a new file - \""<<path<<"\"\nExiting\n";
        exit(1);
    }
    for(const auto& it:mapping)
    {
        fout<<it.first<<"\n"<<it.second<<"$\n";
        // Word terminated by a '\n'..
        // Meaning terminated by a "0\n"..
    }
    fout.close();
}

inline void show(const map<string,string> &mapping=dictionary,int at_a_time=0)
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

inline string beforeSpace(const string &query)  // Retrieve the part of "s" before a " " is encountered..
{
    stringstream ss(query);
    string ans="";
    ss>>ans;
    return ans;
}

inline string afterSpace(const string &query)  // Retrieve the part of "s" after a " " is encountered..
{
    stringstream ss(query);
    string ans="";
    ss>>ans>>ans;
    return ans;
}

inline bool isReserved(string query)  // Check if the query recieved is a keyword ..
{
    transform(query.begin(),query.end(),query.begin(),::tolower);
    static set<string> reserved= {"help","clear","rebuild","show","bak","showbak","loadbak","test","refresh","remove"};
    return reserved.find(beforeSpace(query))!=reserved.end();
}

void showHelp() // Show the help menu..
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

void rebuild()  // Empty the dictionary..
{
    dictionary.clear();
    cout<<"Dictionary cleared!!\n\n";
}

void removeWord(const string& word) // Remove a word-meaning pair from the dictionary..
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

void showRand() // Show a random word-meaning pair from the dictionary..
{
    if(!dictionary.size()) // Dictionary is empty..
        return;
    decltype(dictionary.begin()) it=dictionary.begin();
    advance(it,rand()%dictionary.size());
    cout<<"\t\t\t\""<<it->first<<"\"\n\t\tPress <Enter> to show meaning..  ";
    cin.get();  // Wait for an "enter"...
    cout<<"\n\n"<<it->second<<"\n";
}

void process(const string &query)  // Run the query..
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
        int system_return_value=system(clear_screen);
        // Add checking if system_return_value is OK or not...
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

inline string getQuery()  // Input a Word/Query..
{
    string query="";
    cout<<"\nEnter query..  ";
    getline(cin,query,'\n');
    return query;
}

inline string getMeaning()  // Input the meaning of a word..
{
    string meaning="";
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

void addWord(const string &word) // Add a word to the dictionary..
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
            if(temp!=string("\n"))
                it->second=temp;
        }
    }
    else
    {
        string temp=getMeaning();
        if(temp!=string("\n"))
            dictionary[word]=temp;
    }
}


// Main starts from here...
int main()
{
    srand(time(NULL)); // For showRand()...
    checkConfig();
    readFromFile(dict_path,dictionary);
    while(1)
    {
        string query=getQuery();
        if(query=="") // Exit if the query entered is blank...
            break;
        else if(isReserved(query))
            process(query);
        else
            addWord(query);
    }
    writeToFile(dict_path,dictionary);

}
