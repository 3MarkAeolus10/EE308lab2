#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib> 
#include <stack>
#include <vector>

using namespace std;
const string keywords[] = {"auto","break","case","char","const","continue","default","do","double","else","enum","extern","float","for","goto","if","int","long","register","return","short","signed","sizeof","static","struct","switch","typedef","union","unsigned","void","volatile","while"};
vector<string> file_content;//Build vector containers for keywords

void file_read(string filename){ //build a function to read content 
	ifstream inFile;
	string line;
	inFile.open(filename.c_str());//open the specified file
	if (!inFile.is_open()){
		cout << "The file was not successfully opened and check your file."<< endl;
		exit(1);
	} //Check that the file is opened correctly
	while (getline(inFile, line)) {
        file_content.push_back(line);
    } //Read in line by line
    inFile.close(); //close file
}

void del_useless_cont(){ // Delete the interference content such as comments in the read file. Therefore, the useless content deletion function is established
	
	string Line,words;
	int index;
	size_t tabs_loc, annotation_loc, quot_loc, star1_loc, star2_loc;
	
	for(int i=0;i<file_content.size(); i++){
		Line=file_content[i];
		if(Line.find("\t") != Line.npos && Line.find("\t") == 0){
			do{
				tabs_loc = Line.find("\t")+1;
				Line = Line.substr(tabs_loc,Line.length()-tabs_loc);
			}while(Line.find("\t") != Line.npos && Line.find("\t") == 0);
			file_content[i] = Line;
		}
		if (Line.find("//") != Line.npos) {    
            annotation_loc = Line.find("//");       
            if (annotation_loc == 0) {
                file_content.erase(file_content.begin()+i);      
                i--;
            } else {
                words = Line.substr(0,annotation_loc);       
                file_content[i] = words;
            }
        }else if(Line.find("\"") != Line.npos){
        	size_t temp[50] = {0};
        	tabs_loc = 0;
        	index = 0;
        	while ((quot_loc = Line.find("\"",tabs_loc)) != Line.npos) {
        		temp[index] = quot_loc;
        		index++;
        		tabs_loc = quot_loc +1;
			}
			temp[index] = Line.length();
			words= Line.substr(0,temp[0]);
        	for (int j=1; temp[j]!=0; j+=2) {
                words += Line.substr(temp[j]+1,temp[j+1]-temp[j]-1);      
            }
			file_content[i]=words;	
		} else if(Line.find("/*") != Line.npos){
			star1_loc = Line.find("/*");
            star2_loc = Line.find("*/");
            if (star2_loc != Line.npos) {   
                if (star1_loc == 0) {
                    file_content[i] = Line.substr(star2_loc+2,Line.length());
                } else {
                    file_content[i] = Line.substr(0,star1_loc);
                }
                i--;
            }else{
            	file_content[i] = Line.substr(0,star1_loc);
                i += 1;
                Line = file_content[i];
                while (Line.find("*/") == Line.npos) {
                    file_content.erase(file_content.begin()+i);     
                    Line = file_content[i];
                }
                star2_loc = Line.find("*/")+2;
                file_content[i] = Line.substr(star2_loc,Line.length()-star2_loc);
			}
			
		}
	}
}

bool judge_keywords(string line,long i){//judge keywords, avoid keywords mixed in other words.
	if (line[i] < 48 || (line[i] > 57 && line[i] < 65) || (line[i] > 90 && line[i] < 97) ||line[i] >122) {
        return true;
    } else {
        return false;
    }
}

bool optimize_judge(string line, string word){
    size_t word_loc;
    word_loc = line.find(word);
    if (word_loc == 0) {
        if (judge_keywords(line, word.length()) || word.length() == line.length()) {
            return true;
        } else {
            return false;
        }
    } else {
        if (judge_keywords(line, word_loc-1) && (judge_keywords(line, word_loc+word.length()) || word_loc+word.length() == line.length())) {
            return true;
        } else {
            return false;
        }
    }
    
}

void cal_keywords_num(){// calculate the number of keywords
	int keywords_num=0;
	string line;
	for (int i = 0; i<file_content.size(); i++) {
        line = file_content[i];
        for (int j=0; j<32; j++) {
            size_t keywords_loc = line.find(keywords[j], 0);
            while (keywords_loc!=line.npos && optimize_judge(line,keywords[j])) {
                keywords_num++;
                keywords_loc = line.find(keywords[j],keywords_loc + 1);
            }
        }
    }
    cout<<"total num: "<<keywords_num<<endl;
}

void cal_switch_case_num(){// calculate the number of "switch-case" structure
    int switch_num = 0,last = -1,case_num[100]={0};
    string line;
    for (int i = 0; i<file_content.size(); i++) {
        line = file_content[i];
        if (line.find("switch") != line.npos && optimize_judge(line, "switch")) {        
            switch_num += 1;
            last += 1;
        }
        if (line.find("case") != line.npos && optimize_judge(line, "case")) {      
            case_num[last] += 1;
        }
    }
    cout<<"switch num: "<<switch_num<<endl;
    cout<<"case num:";
    for (int j = 0; j<=last; j++) {
        cout<<" "<<case_num[j];
    }
    cout<<endl;	
}

void cal_if_else_num(int level) {// calculate the number of "if-else" structure
    int if_else_num = 0, if_elseif_else_num= 0;
    stack<int> s;
    string line;
    for (int i = 0; i<file_content.size(); i++) {
        line = file_content[i];
        if (line.find("if") != line.npos && line.find("else") == line.npos && optimize_judge(line, "if")) {      
            s.push(1);
        } else if (line.find("if") == line.npos && line.find("else") != line.npos &&  s.empty() == false) {
            if (s.top() == 1) {
                if_else_num++;    
            } else {
                if_elseif_else_num++;    
            }
            s.pop();       
            
        }else  if (line.find("if") != line.npos && line.find("else") != line.npos && optimize_judge(line, "if")) {
            s.push(2);    
        }
    }

	cout<<"if-else num: "<<if_else_num<<endl;
    cout<<"if-else-if num: "<<if_elseif_else_num<<endl;
    
}

int main() {
    int level = 0;
    string filename;
    cout<<"Please enter the path of the code file: "<<endl;
	cin>>filename;
    
    file_read(filename);
    del_useless_cont();  
    cal_keywords_num();
    cal_switch_case_num();
    cal_if_else_num(4);
    
    return 0;
}
