#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

struct Topic{
    string title;
    string shortDesc;
    string detailDesc;
    string imagePath;
    string keywords;
};

struct TrieNode{
    TrieNode* children[26];
    bool isEnd;

    TrieNode(){
        isEnd = false;
        for(int i=0;i<26;i++)
            children[i] = NULL;
    }
};

string toLower(string s){
    for(char &c : s){
        c = tolower(c);
    }
    return s;
}

void insert(TrieNode* root, string word){
    TrieNode* node = root;

    for(char c : word){
        int index = c - 'a';

        if(node->children[index] == NULL){
            node->children[index] = new TrieNode();
        }

        node = node->children[index];
    }

    node->isEnd = true;
}

TrieNode* searchPrefix(TrieNode* root, string prefix){
    TrieNode* node = root;

    for(char c : prefix){
        int index = c - 'a';

        if(node->children[index] == NULL)
            return NULL;

        node = node->children[index];
    }

    return node;
}

void getSuggestions(TrieNode* node, string prefix){
    if(node->isEnd){
        cout << prefix << endl;
    }

    for(int i=0;i<26;i++){
        if(node->children[i] != NULL){
            char nextChar = 'a' + i;
            getSuggestions(node->children[i], prefix + nextChar);
        }
    }
}

int main(){

    ifstream file("topics.txt");
    string line;
    vector<Topic> topics;

    if(!file){
        cout<<"File not found"<<endl;
        return 0;
    }

    while(getline(file,line)){
        if(line.empty()) continue;

        stringstream ss(line);
        Topic t;

        getline(ss, t.title, '|');
        getline(ss, t.shortDesc, '|');
        getline(ss, t.detailDesc, '|');
        getline(ss, t.imagePath, '|');
        getline(ss, t.keywords, '|');

        topics.push_back(t);
    }

    file.close();

    cout<<"Total topics loaded: "<<topics.size()<<endl;

    TrieNode* root = new TrieNode();

    for(int i = 0; i < topics.size(); i++){
        string key = toLower(topics[i].keywords);
        stringstream ss(key);
        string word;

        while(ss >> word){
            insert(root, word);
        }
    }

    int choice;

    do{
        cout<<"\n===== MENU =====\n";
        cout<<"1. Show All Topics\n";
        cout<<"2. Autocomplete Suggestions\n";
        cout<<"3. Search Topics\n";
        cout<<"4. Exit\n";
        cout<<"Enter your choice: ";
        cin>>choice;
        cin.ignore();

        if(choice == 1){
            for(int i = 0; i < topics.size(); i++){
                cout<<"\nTopic: "<<topics[i].title<<endl;
                cout<<"Short Description: "<<topics[i].shortDesc<<endl;
                cout<<"Detailed Description: "<<topics[i].detailDesc<<endl;
                cout<<"-----------------------------------"<<endl;
            }
        }

        else if(choice == 2){
            string prefix;
            cout<<"\nEnter prefix: ";
            getline(cin, prefix);

            prefix = toLower(prefix);

            TrieNode* node = searchPrefix(root, prefix);

            if(node == NULL){
                cout<<"No suggestions found."<<endl;
            }else{
                cout<<"\nSuggestions:\n";
                getSuggestions(node, prefix);
            }
        }

        else if(choice == 3){
            string query;
            cout<<"\nEnter search query: ";
            getline(cin, query);

            query = toLower(query);

            cout<<"\nSearch Results:\n";

            vector<string> printed;
            bool found = false;

            for(int i = 0; i < topics.size(); i++){

                string key = toLower(topics[i].keywords);
                string title = toLower(topics[i].title);

                if(key.find(query) != string::npos || title.find(query) != string::npos){

                    bool alreadyPrinted = false;
                    for(string t : printed){
                        if(t == topics[i].title){
                            alreadyPrinted = true;
                            break;
                        }
                    }

                    if(!alreadyPrinted){
                        cout<<"\nTopic: "<<topics[i].title<<endl;
                        cout<<"Short Description: "<<topics[i].shortDesc<<endl;

                        printed.push_back(topics[i].title);
                        found = true;
                    }
                }
            }

            if(!found){
                cout<<"No matching topics found."<<endl;
            }
        }

        else if(choice == 4){
            cout<<"Exiting program..."<<endl;
        }

        else{
            cout<<"Invalid choice!"<<endl;
        }

    }while(choice != 4);

    return 0;
}
