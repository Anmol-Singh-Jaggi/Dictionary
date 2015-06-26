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
#include<ctime>

#define BOOST_FILESYSTEM_NO_DEPRECATED
#include <boost/filesystem.hpp>

using  namespace std;
using namespace boost::filesystem;

#if BOOST_OS_LINUX // Linux
#define PLATFORM_CONFIG_PATH    (getenv("HOME")+string("/.dict_config.txt"))
#define PLATFORM_RESET_COMMAND  "reset"

#elif BOOST_OS_WINDOWS // Windows
#define PLATFORM_CONFIG_PATH    (getenv("USERPROFILE")+string("\\My Documents\\dict_config.txt"))
#define PLATFORM_RESET_COMMAND  "cls"

#else
#error "Untested platform"
#endif


class Dictionary
{

	/************************************************/
	/************* Private data members *************/
	/************************************************/

	// OS-dependent command to clear the terminal screen
	const string clearScreenCommand = PLATFORM_RESET_COMMAND;

	// Path to the configuration file.
	// The configuration file stores the path to the dictionary file.
	const path configFilePath = PLATFORM_CONFIG_PATH;

	// The data structure for containing the dictionary word-meaning pairs in the memory
	// A "word" can be a multi-word string unlike what its name suggests
	// A meaning can be a multi-line string
	map<string, string> dict;

	// A file handle to the dictionary file
	fstream dictFile;

	// Path to the dictionary file.
	// The dictionary file stores the word-meaning pairs.
	path dictFilePath;

	// The data structure for holding the help keywords and their explanations
	map<string, string> helpMap;

	/****************************************************/
	/************* Private member functions *************/
	/****************************************************/

	// Empty the dictionary
	void Clear();

	// Display the contents of the dictionary "num" pairs at a time
	void Display( size_t num = 0 );

	// Retrieve the first word of the query string
	string FirstWord( const string& query );

	// Helper for the constructor
	// Initialize helpMap
	void InitHelp();

	// Helper for the constructor
	// Read the path to the dictionary file from the config file.
	void ReadDictFilePathFromConfigFile();

	// Helper for the constructor
	// Load the contents of the dictionary file.
	void ReadFromDictFile();

	// Input the meaning of a word
	string ReadMeaning();

	// Remove a word-meaning pair from the dictionary
	void Remove( const string& word );

	// Retrieve the second word of the query string
	string SecondWord( const string& query );

	// Helper for the constructor
	// Make sure that the configuration file is existing.
	// If it is not existing, then initialize it by writing to it the path to the dictionary file.
	void SetupConfigFile();

	// Helper for the constructor
	// Make sure that the dictionary file exists, if not then create a new one.
	void SetupDictFile();

	// Show the help menu
	void ShowHelp();

	// Show a random pair from the dictionary
	void TakeTest();

	// Utility function
	// Convert any type to its string representation
	template<typename T> string ToString( const T& obj );

public:

	/***************************************************/
	/************* Public member functions *************/
	/***************************************************/

	// Read the dictionary file-path from the config file and populate the map from it.
	Dictionary();

	// Write the dictionary map to the dictionary file
	~Dictionary();

	// Perform the action associated with the reserved keyword
	void DoReserved( const string& query );

	// Add a word to the dictionary
	void Insert( const string& word );

	// Check if the query recieved is a keyword
	bool IsReserved( string query );

	// Input a query
	string ReadQuery();
};


/*******************************************************************/
/************* Private member function implementations *************/
/*******************************************************************/

void Dictionary::Clear()
{
	dict.clear();
	cout << "Dictionary cleared!!\n\n";
}

void Dictionary::Display( size_t num )
{
	cout << "\t\t\t" << "Number of entries = " << dict.size() << "\n";

	// Show all the word-meaning pairs at once
	if ( num == 0 )
	{
		num = dict.size();
	}

	size_t i = 1;
	for ( const auto& it : dict )
	{
		cout << i << "). " << it.first << "\n\n" << it.second << "\n\n";

		if ( ( i % num == 0 ) && i != dict.size() )
		{
			cout << "\t\tPress <Enter> to resume..";
			cin.get();  // Wait for the user
		}
		++i;
	}
}

string Dictionary::FirstWord( const string& query )
{
	stringstream ss( query );
	string ans;
	ss >> ans;
	return ans;
}

void Dictionary::InitHelp()
{
	helpMap["help"] = "Show this help screen";
	helpMap["screen"] = "Clear screen.";
	helpMap["clear"] = "Delete all the entries in the dictionary.";
	helpMap["show"] = "Show all the entries in the dictionary. Usage - 'show [number of words at a time]'";
	helpMap["test"] = "Take a self-test by showing any randomly chosen word from the dictionary and making you guess its meaning.";
	helpMap["remove"] = "Remove a word. Usage - 'remove <word>'";
	helpMap["exit"] = "Exit after saving the dictionary safely to the file";
}

void Dictionary::ReadDictFilePathFromConfigFile()
{
	ifstream fin( configFilePath.string() );
	// This will happen if there is a race condition and some other process deletes the config file just after we close it above.
	if ( !fin )
	{
		const string errorMsg = "Line " + ToString( __LINE__ ) + " : Error in reading '" + configFilePath.string() + "' : " + strerror( errno );
		throw errorMsg;
	}

	cout << "Trying to read the path of the dictionary file from the config file." << endl;

	// Read the dictionary file path from the config file
	string dictFilePathString;
	getline( fin, dictFilePathString );
	dictFilePath = path( dictFilePathString );
	cout << "The path of the dictionary file is - " << dictFilePath << endl;
}

void Dictionary::ReadFromDictFile()
{
	cout << "Opening the dictionary file" << endl;

	dictFile.open( dictFilePath.string(), ios_base::in );
	if ( !dictFile )
	{
		const string errorMsg = "Line " + ToString( __LINE__ ) + " : Error in opening '" + dictFilePath.string() + "' : " + strerror( errno );
		throw errorMsg;
	}

	cout << "Reading the contents of the dictionary from the dictionary file." << endl;

	// Read the contents of the dictionary file and load it in the dictionary data structure
	string word, meaning;
	while ( getline( dictFile, word, '$' ) )
	{
		dictFile.get(); // Read the newline after word
		if ( ( !word.empty() ) && getline( dictFile, meaning, '$' ) && ( !meaning.empty() ) )
		{
			dict.insert( make_pair( word, meaning ) );
		}
		dictFile.get(); // Read the newline after meaning

		// Maybe we missed a '$' here or there
		if ( !dictFile )
		{
			const string errorMsg = "Line " + ToString( __LINE__ ) + " : Error in reading '" + dictFilePath.string() + "' : The dictionary is in an inconsistent state";
			throw errorMsg;
		}
	}
	dictFile.close();
}

string Dictionary::ReadMeaning()
{
	string meaning;
	cout << "\nEnter meaning -:\n";
	char c;
	while ( 1 )
	{
		cin.get( c );
		// If the character read is a newline and the previous character read was also a newline.
		if ( meaning.size() && meaning[meaning.size() - 1] == '\n' && c == '\n' )
		{
			break;
		}
		meaning += c;
	}
	if ( meaning.find( '$' ) != string::npos )
	{
		cout << "The input cannot contain '$'" << endl;
		meaning = "\n"; // so that Dictionary::Insert() ignores it
	}
	return meaning;
}

void Dictionary::Remove( const string& word )
{
	decltype( dict.begin() ) it;
	if ( ( it = dict.find( word ) ) != dict.end() )
	{
		dict.erase( it );
		cout << "'" << word << "' erased.\n";
	}
	else
	{
		cout << "'" << word << "' not found!\n";
	}
}


string Dictionary::SecondWord( const string& query )
{
	stringstream ss( query );
	string ans;
	ss >> ans >> ans;
	return ans;
}

void Dictionary::SetupConfigFile()
{
	cout << "The configuration file is not present.\nCreating a new configuration file.\n";

	// Create a new config file
	ofstream fout( configFilePath.string() );

	// If successful in creating the config file
	if ( fout )
	{
		cout << "Configuration file created successfully !\n";

		cout << "Enter the path to store the dictionary file -:\n";
		string dictFilePathString;
		getline( cin, dictFilePathString );
		dictFilePath = path( dictFilePathString );

		// Remove all symbolic links from the path
		try
		{
			dictFilePath = canonical( dictFilePath.parent_path() ) / dictFilePath.filename();
		}
		catch ( const filesystem_error& ex )
		{
			const string errorMsg = "Line " + ToString( __LINE__ ) + " : Dictionary path '" + dictFilePath.string() + "' not valid : " + ex.what();
			throw errorMsg;
		}

		// Write the dictionary file path in the config file.
		fout << dictFilePath.generic_string() << endl;
	}
	else
	{
		const string errorMsg = "Line " + ToString( __LINE__ ) + " : Error in creating '" + configFilePath.string() + "' : " + strerror( errno );
		throw errorMsg;
	}

	cout << "Path to the dictionary file successfully saved into the configuration file.\n";
}

void Dictionary::SetupDictFile()
{
	cout << "No dictionary file present." <<  endl;

	// Create a new dictionary file
	dictFile.open( dictFilePath.string(), ios_base::out );

	// If not successful in creating a new dictionary file
	if ( !dictFile )
	{
		const string errorMsg = "Line " + ToString( __LINE__ ) + " : Error in creating '" + dictFilePath.string() + "' : " + strerror( errno );
		throw errorMsg;
	}

	dictFile.close();
	cout << "New dictionary file created successfully!!" << endl;
}

void Dictionary::ShowHelp()
{
	cout << "\n";
	for ( const auto& it : helpMap )
	{
		cout << "'" << it.first << "' -- " << it.second << "\n";
	}
	cout << endl;
}

void Dictionary::TakeTest()
{
	if ( !dict.size() ) // Dictionary is empty
	{
		return;
	}

	decltype( dict.begin() ) it = dict.begin();
	advance( it, rand() % dict.size() );

	cout << "\t\t\t'" << it->first << "'\n\t\tPress <Enter> to show meaning..";
	cin.get();  // Wait for an "enter"
	cout << "\n\n" << it->second << "\n";
}

template<typename T> string Dictionary::ToString( const T& obj )
{
	stringstream ss;
	ss << obj;
	return ss.str();
}

/******************************************************************/
/************* Public member function implementations *************/
/******************************************************************/

Dictionary::Dictionary()
{
	InitHelp();

	// If the configuration file does not exists, then setup the config file
	if ( !exists( configFilePath ) )
	{
		SetupConfigFile();
	}

	ReadDictFilePathFromConfigFile();

	// If the dictionary file does not exist
	if ( !exists( dictFilePath ) )
	{
		SetupDictFile();
	}

	ReadFromDictFile();

	// Open the dictionary file one last time for the duration of the object. It will be closed only in the destructor.
	dictFile.open( dictFilePath.string(), ios_base::out );

	// Should fail only in if some race-condition occurs
	if ( !dictFile )
	{
		const string errorMsg = "Line " + ToString( __LINE__ ) + " : Error in opening '" + dictFilePath.string() + "' : " + strerror( errno );
		throw errorMsg;
	}
}

Dictionary::~Dictionary()
{
	if ( exists( dictFilePath ) && dictFile )
	{
		cout << "Saving the dictionary's contents into the file" << endl;
		for ( const auto& it : dict )
		{
			// words and meanings both are terminated by "$\n"
			dictFile << it.first << "$\n" << it.second << "$\n";
		}
	}
	cout << "File saved!!" << endl;
}

void Dictionary::DoReserved( const string& query )
{
	string beforeSpace = FirstWord( query );
	string afterSpace = SecondWord( query );
	transform( beforeSpace.begin(), beforeSpace.end(), beforeSpace.begin(), ::tolower );

	if ( beforeSpace == "help" )
	{
		ShowHelp();
	}
	else if ( beforeSpace == "clear" )
	{
		Clear();
	}
	else if ( beforeSpace == "show" )
	{
		Display( atoi( afterSpace.c_str() ) );
	}
	else if ( beforeSpace == "screen" )
	{
		system( clearScreenCommand.c_str() );
	}
	else if ( beforeSpace == "test" )
	{
		TakeTest();
	}
	else if ( beforeSpace == "remove" )
	{
		Remove( afterSpace );
	}
}

void Dictionary::Insert( const string& word )
{
	decltype( dict.begin() ) it;

	string option;
	// Word already present in the dictionary
	if ( ( it = dict.find( word ) ) != dict.end() )
	{
		cout << "'" << it->first << "' already exists!!\n\n" << it->second << "\n\nWant to overwrite?? (y/n)  ";
		getline( cin, option );
		if ( option.size() && option[0] == 'y' )
		{
			string temp = ReadMeaning();
			if ( temp != "\n" )
			{
				it->second = temp;
				cout << "Definition of '" << word << "' updated!!" << endl;
			}
			else
			{
				cout << "'" << word  << "' not added!" << endl;
			}
		}
	}
	// New addition
	else
	{
		string temp = ReadMeaning();
		if ( temp != "\n" )
		{
			dict[word] = temp;
			cout << "New definition of '" << word << "' added!!" << endl;
		}
		else
		{
			cout << "Word not added!" << endl;
		}
	}
}

bool Dictionary::IsReserved( string query )
{
	transform( query.begin(), query.end(), query.begin(), ::tolower );
	return helpMap.count( FirstWord( query ) );
}

string Dictionary::ReadQuery()
{
	string query;
	cout << "\nEnter query - ";
	getline( cin, query, '\n' );
	if ( query.find( '$' ) != string::npos )
	{
		cout << "The input cannot contain '$'" << endl;
		query = ""; // so that main() ignores it
	}
	return query;
}


/*****************************************************/
/************* Class definition ends *************/
/*****************************************************/


int main()
{
	try
	{
		srand( time( NULL ) );
		Dictionary dictionary;
		while ( 1 )
		{
			string query = dictionary.ReadQuery();
			if ( query.empty() )
			{
				continue;
			}
			else if ( query == "exit" )
			{
				break;
			}
			else if ( dictionary.IsReserved( query ) )
			{
				dictionary.DoReserved( query );
			}
			else
			{
				dictionary.Insert( query );
			}
		}
	}
	catch ( const string& errorMsg )
	{
		cerr << errorMsg << endl;
		return -1;
	}
}
