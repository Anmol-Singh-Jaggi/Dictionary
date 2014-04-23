#include<bits/stdc++.h>
using  namespace std;

map<string,string> dictionary,dictionary_bak;  // Store word-meaning pairs...

string config_path=string(getenv("HOME"))+string("/.dict_config.txt");  // Path to store configuration File...
// Configuration File consists of the location of dictionary and dictionary_bak..

char path[1000],bak_path[1000];
// path stores the path of the dictionary..
// bak_path stores the path of the backup dictionary..

void checkConfig()  // Check if the dictionary can be loaded..
{
    string s_path,s_bak_path;
    ifstream fin(config_path.c_str());
    cout<<"Checking the location of the configuration file....  ";
    if(fin.good()) // Configuration File opened successfully..
    {
        cout<<"Configuration File present..\n";
        getline(fin,s_path);  // Get the location of the dictionary..
        getline(fin,s_bak_path);    // Get the location of the backup dictionary..
    }
    else
    {
        cout<<"Configuration File not present..\n";
    }
    cout<<"Checking the location of the dictionary file....  ";
    ofstream fout2(s_path.c_str(),fstream::app);
    ifstream fin2(s_path.c_str());
    while(!fin2.good())  // Ask where to store dictionary..
    {
        cout<<"Invalid Path!\nEnter the path to store dictionary\nFor example - /home/anmol/Documents/dictionary.txt\n";
        getline(cin,s_path);
        fout2.open(s_path.c_str(),fstream::app);
        fin2.open(s_path.c_str());
    }
    cout<<"Dictionary File at a valid location..\n";
    fin2.close();
    fout2.close();
    cout<<"Checking the location of the backup dictionary file....   ";
    ofstream fout2_bak(s_bak_path.c_str(),fstream::app);
    ifstream fin2_bak(s_bak_path.c_str());
    while(!fin2_bak.good())  // Ask where to store dictionary_bak..
    {
        cout<<"Invalid Path!\nEnter the path to store dictionary backup\nFor example - /home/anmol/Documents/dictionary_bak.txt\n";
        getline(cin,s_bak_path);
        fout2_bak.open(s_bak_path.c_str(),fstream::app);
        fin2_bak.open(s_bak_path.c_str());
    }
    cout<<"Dictionary Backup File at a valid location..\n";
    fin2_bak.close();
    fout2_bak.close();
    cout<<"Writing the location of dictionary and backup dictionary to the configuration file...   ";
    ofstream fout(config_path.c_str());
    fout<<s_path<<"\n"<<s_bak_path;
    if(fout.good())
        cout<<"Successful!\n";
    else
        cout<<"Error writing to the config file!!\n"; // Shouldn't happen usually..
    fout.close();
    strcpy(path,s_path.c_str());  // Copy the contents of s_path into path..
    strcpy(bak_path,s_bak_path.c_str());  // Copy the contents of s_bak_path into bak_path..
    fin.close(); // Close the config file..
}

void readFromFile(const char* filePath,map<string,string> &mapping)  // Get the contents of the dictionary from "filePath" and store it in memory as a mapping..
{
    //assert(!system((string("dos2unix -q ")+filePath).c_str()));
    string word,meaning;
    ifstream fin(filePath);
    mapping.clear();
    if(fin.good())  // File is opened successfully..
    {
        while(1)
        {
            getline(fin,word,'\n'); // A word is terminated by a '\n'..
            getline(fin,meaning,'0');  // A meaning is terminated by a '0'...
            fin.get(); // Take the remaining '\n'..
            if(fin.eof())  // End-Of-File reached..
            {
                fin.clear();
                break;
            }
            mapping[word]=meaning;
        }
        fin.close();
    }
    else  // File not opened successfully..
    {
        string error=string("\n\nFatal error - ")+filePath;
        perror(error.c_str());  // Output an error..
        cout<<"Looks like the path for the dictionary you entered is invalid.\nPlease restart the program!! \n\n";
        remove(config_path.c_str()); // Remove the configuration file..
        exit(0);  // Close the program...
    }
}

inline void writeToFile(const char* filePath,map<string,string> &mapping) // Save the dictionary to the file..
{
    ofstream fout;
    fout.open(filePath);
    for(const auto& it:mapping)
    {
        fout<<it.first<<"\n"<<it.second<<"0\n";
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
        if((i%at_a_time==0)&&i!=(int)mapping.size())
            cin.get();
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

inline bool isReserved(const string &query)  // Check if the query recieved is a keyword ..
{
    static set<string> reserved= {"help","reset","rebuild","show","bak","showbak","loadbak","rand","refresh","remove"};
    return reserved.find(beforeSpace(query))!=reserved.end();
}

inline void showHelp() // Show the help screen..
{
    static map<string,string> help;
    help["\n\"reset\""]="Clear screen...\n";
    help["\n\"rebuild\""]="Delete all the present entries of the dictionary...\n";
    help["\n\"show [no. of entries at a time]\""]="Show all the present entries of the dictionary...\n";
    help["\n\"bak\""]="Backup all the present entries of the dictionary...\n";
    help["\n\"showbak [no. of entries at a time]\""]="Show all the present entries of the Backup dictionary...\n";
    help["\n\"loadbak\""]="Replace the current dictionary with the most recent backup...\n";
    help["\n\"rand\""]="Show any randomly chosen word from the dictionary...\n";
    help["\n\"refresh\""]="Reload the dictionary from the file...\n";
    help["\n\"remove <word>\""]="Remove a word...\n";
    for(const auto& it:help)
    {
        cout<<it.first<<" -- "<<it.second;
    }
}

inline void rebuild()  // Empty the dictionary..
{
    dictionary.clear();
    cout<<"Dictionary cleared!!\n\n";
}

inline void removeWord(const string& word) // Remove a word-meaning pair from the dictionary..
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

inline void showRand() // Show a random word-meaning pair from the dictionary..
{
    if((int)dictionary.size()<=0) // Dictionary is empty..
        return;
    decltype(dictionary.begin()) it=dictionary.begin();
    advance(it,rand()%dictionary.size());
    cout<<"\t\t\t\""<<it->first<<"\"\n";
    cin.get();
    cout<<it->second<<"\n";
}

void process(const string &query)  // Run the query..
{
    string before_space=beforeSpace(query);
    string after_space=afterSpace(query);
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
    else if(before_space=="reset")
    {
        assert(!system("reset"));
    }
    else if(before_space=="bak")
    {
        writeToFile(bak_path,dictionary);
    }
    else if(before_space=="showbak")
    {
        readFromFile(bak_path,dictionary_bak);
        show(dictionary_bak,atoi(after_space.c_str()));
    }
    else if(before_space=="loadbak")
    {
        readFromFile(bak_path,dictionary);
        writeToFile(path,dictionary);
    }
    else if(before_space=="rand")
    {
        showRand();
    }
    else if(before_space=="remove")
    {
        removeWord(after_space);
    }
    else if(before_space=="refresh")
    {
        readFromFile(path,dictionary);
    }
}

inline string getQuery()  // Input a Word/Query..
{
    string query="";
    cout<<"\nEnter query.. [Type \"help\" for a list of commands] ";
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
        char c;
        cin>>c;
        cin.ignore(numeric_limits<streamsize>::max(),'\n');
        if(c=='y'||c=='Y')
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
    readFromFile(path,dictionary);
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
    writeToFile(path,dictionary);
}
