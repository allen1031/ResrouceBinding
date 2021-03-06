// ConsoleApplication3.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <cmath>
#include <cstdio>
#include <vector>
#include <iostream>
#include <string>
#include <algorithm>
#include <sstream>
#include <map>
#include <limits>
#include <climits>
using namespace std;

/*********************Data Structure***************************/
class BasicBlock {

protected:
	char symbol;
	int instruct_latency;
	

public:
	int map_seq;
	bool scheduled;
	void setOperator(char sym) { symbol = sym; }
	char getOperator() { return symbol; }

	int getInstructionLatency() {


		if (symbol == '+' || symbol == '-') {
			instruct_latency = 1;
		}

		if (symbol == '*' || symbol == '/') {
			instruct_latency = 4;
		}

		return instruct_latency;
	}
};


class Instruction:public BasicBlock{

private:

	vector<Instruction*> parents;
	vector<Instruction*> users;
	
public:
	int pos;
	bool ifconsumed;
	Instruction() {}

	vector<Instruction*> addParent(Instruction *I) { parents.push_back(I);  return parents; }
	vector<Instruction*> addUsers(Instruction *I) { users.push_back(I); return users; }
	vector<Instruction*> getParent() { return parents; }
	vector<Instruction*> getUsers() { return users; }


};



/*********************Input Equation Parsing******************/
vector<Instruction> InstructionBlock;  //Unmapped Operator Instructions
vector<Instruction> Mapped_operators;  //Mapped Operator Instructions
size_t mapped = 0;                  //Number of operator to be mapped
int cycle = 0;                         //The run cycle of the current operator.
vector<int> record;
size_t i;
size_t j;

//Function to map parent and children node to each operator
void findUser(int index,vector<Instruction> *Block) {

	size_t user_left = index;
	size_t user_right = index;
	

	if ((*Block).size() > 1) {
		
		if (index == 0) {     //operator at beginning
			do {
				user_right++;
				if ((*Block)[user_right].ifconsumed == 0) {
					(*Block)[index].addUsers(&(*Block)[user_right]);
					(*Block)[user_right].addParent(&(*Block)[index]);
					//cout << "begin" << endl;
					break;
				}
				if (user_right >= (*Block).size()-1) {
					break;
				}
			} while (1);
			
		}
		else if (index == (*Block).size() - 1) {   //operator at end
			do {
				user_left--;
				if ((*Block)[user_left].ifconsumed == 0) {
					(*Block)[user_left].addParent(&(*Block)[index]);
					(*Block)[index].addUsers(&(*Block)[user_left]);
					//cout << "end" << endl;
					break;
				}
				if (user_left <= 0) {
					break;
				}
			} while (1);
			
		}
		else {                                //operator in the middle

			//cout << "mid" << endl;
			do {
				user_right++;
				if ((*Block)[user_right].ifconsumed == 0) {
					break;
				}
				if (user_right >= (*Block).size()-1) {
					user_right = index;
					break;
				}
			} while (1);


			do {
				user_left--;
				if ((*Block)[user_left].ifconsumed == 0) {
					break;
				}

				if (user_left <= 0) {
					user_left = index;
					break;
				}
			} while (1);
			
			if (user_left == index && user_right != index) {
				(*Block)[user_right].addParent(&(*Block)[index]);
				(*Block)[index].addUsers(&(*Block)[user_right]);
			}
			else if (user_left != index && user_right == index) {
				(*Block)[user_left].addParent(&(*Block)[index]);
				(*Block)[index].addUsers(&(*Block)[user_left]);
			}
			else if (user_left != index && user_right != index) {
			
				if ((*Block)[user_right].getOperator() == '*' || (*Block)[user_right].getOperator() == '/') {
					(*Block)[user_right].addParent(&(*Block)[index]);
					(*Block)[index].addUsers(&(*Block)[user_right]);
				}
				else {
					(*Block)[user_left].addParent(&(*Block)[index]);
					(*Block)[index].addUsers(&(*Block)[user_left]);
				}
			}
			else {} 
		}

		

	}
	
}

bool wayToSort(Instruction* I, Instruction* J) { 
		return (*I).map_seq < (*J).map_seq; 
}

// Create a related map for the operator equation
vector<Instruction*> InstructionParse(string operation) {

	Instruction* op;
	
	// Removal of numbers
	for (size_t i = 0; i < operation.length(); i++) {
		if (   operation[i] == '(' 
			|| operation[i] == ')'
			|| operation[i] == '+'
			|| operation[i] == '-'
			|| operation[i] == '*'
			|| operation[i] == '/') {

			op = new Instruction();
			op->setOperator(operation[i]);
			InstructionBlock.push_back((*op));

		}
	}
	
	size_t size = InstructionBlock.size();
	mapped = 0;

	//Add id to each operator
	for (i = 0; i < InstructionBlock.size(); i++) {
		InstructionBlock[i].pos = i;
		Mapped_operators.push_back(InstructionBlock[i]);
	}
	//Separate the operators for each run cycle
	while (mapped < size) {
		
		cout << endl;
		cout << "The run cycle "<<cycle << " :"<<endl;
		record.clear();
		for (size_t i = 0; i < InstructionBlock.size(); i++) {

			
			if (InstructionBlock[i].getOperator() == '*' || InstructionBlock[i].getOperator() == '/') {

				if (i - 1 >= 0 && i - 1 < InstructionBlock.size()) {
					if (InstructionBlock[i - 1].ifconsumed == 0) {
						InstructionBlock[i].ifconsumed = 1;
						Mapped_operators[InstructionBlock[i].pos].ifconsumed = 1;
						
						Mapped_operators[InstructionBlock[i].pos].map_seq = mapped;
						mapped++;
						record.push_back(i);
						cout << "condition 1: " << InstructionBlock[i].getOperator() << endl;
						findUser(InstructionBlock[i].pos, &Mapped_operators);
					}
				}
				else if (i == 0) {							 //operator at beginning
					InstructionBlock[i].ifconsumed = 1;
					Mapped_operators[InstructionBlock[i].pos].ifconsumed = 1;
					
					Mapped_operators[InstructionBlock[i].pos].map_seq = mapped;
					mapped++;
					record.push_back(i);
					cout << "condition 2: " << InstructionBlock[i].getOperator() << endl;
					findUser(InstructionBlock[i].pos, &Mapped_operators);
				}
			}
			else if (InstructionBlock[i].getOperator() == '+' || InstructionBlock[i].getOperator() == '-') {


				if (InstructionBlock.size() == 1) {            //only 1 operator exists
					InstructionBlock[i].ifconsumed = 1;
					Mapped_operators[InstructionBlock[i].pos].ifconsumed = 1;
					
					Mapped_operators[InstructionBlock[i].pos].map_seq = mapped;
					mapped++;
					record.push_back(i);
					cout << "condition 3: " << InstructionBlock[i].getOperator() << endl;
					findUser(InstructionBlock[i].pos, &Mapped_operators);
				}
				else if (i == 0) {                               //operator at beginning
					if (InstructionBlock[i + 1].getOperator() == '+' || InstructionBlock[i + 1].getOperator() == '-') {
						InstructionBlock[i].ifconsumed = 1;
						Mapped_operators[InstructionBlock[i].pos].ifconsumed = 1;
						
						Mapped_operators[InstructionBlock[i].pos].map_seq = mapped;
						mapped++;
						record.push_back(i);
						cout << "condition 4: " << InstructionBlock[i].getOperator() << endl;
						findUser(InstructionBlock[i].pos, &Mapped_operators);
					}

				}
				else if (i == InstructionBlock.size() - 1) {        //operator at end
					if (InstructionBlock[i - 1].ifconsumed == 0 && (InstructionBlock[i - 1].getOperator() == '+' || InstructionBlock[i - 1].getOperator() == '-')) {
						InstructionBlock[i].ifconsumed = 1;
						Mapped_operators[InstructionBlock[i].pos].ifconsumed = 1;
						
						Mapped_operators[InstructionBlock[i].pos].map_seq = mapped;
						mapped++;
						record.push_back(i);
						cout << "condition 5: " << InstructionBlock[i].getOperator() << endl;
						findUser(InstructionBlock[i].pos, &Mapped_operators);
					}

				}
				else {                                         //operator in the middle
					if (InstructionBlock[i - 1].ifconsumed == 0 && (InstructionBlock[i - 1].getOperator() == '+' || InstructionBlock[i - 1].getOperator() == '-')) {
						if (InstructionBlock[i + 1].ifconsumed == 0 && (InstructionBlock[i + 1].getOperator() == '+' || InstructionBlock[i + 1].getOperator() == '-')) {
							InstructionBlock[i].ifconsumed = 1;
							Mapped_operators[InstructionBlock[i].pos].ifconsumed = 1;
							
							Mapped_operators[InstructionBlock[i].pos].map_seq = mapped;
							mapped++;
							record.push_back(i);
							cout << "condition 6: " << InstructionBlock[i].getOperator() << endl;
							findUser(InstructionBlock[i].pos, &Mapped_operators);
						}
					}

				}

			}
			
		}

		// Remove mapped operators from mapping instruction.
		std::sort(record.begin(), record.end());
		for (size_t i = 0; i < record.size(); i++) {
			InstructionBlock.erase(InstructionBlock.begin() + record[record.size() - 1 - i]);
		}
		cycle++;

		// Exceptions
		if (cycle > 100) {
			cout << "The mapping takes unreasonably long time." << endl;
			break;
		}
	}

	vector<Instruction*> MappedBlock;
	for (i = 0; i < Mapped_operators.size(); i++) {
		MappedBlock.push_back(&Mapped_operators[i]);
	}
	sort(MappedBlock.begin(), MappedBlock.end(), wayToSort);
	
	return MappedBlock;
}

/*********************ASAP Scheduing***************************/
std::map<Instruction*, int> ASAPschedule;
int ASAPLatency;

int scheduleASAP(vector<Instruction*> BB, std::map<Instruction*, int> schedule) {
	int maxLatency = 0;
	//schedule = {};
	for (Instruction* inst : BB) {
		for (Instruction *I : (*inst).getUsers()) {
			
			schedule[I] = max(schedule[I], schedule[inst] + (*inst).getInstructionLatency());
			maxLatency = max(maxLatency, schedule[I] + I->getInstructionLatency());
			
		}
		cout << "The schedule of " << (*inst).getOperator() << " is " << schedule[inst] << endl;
		maxLatency = max(maxLatency, schedule[inst] + (*inst).getInstructionLatency());
	}
	return maxLatency;
}

/*********************ALAP Scheduing***************************/
std::map<const Instruction *, int> ALAPschedule;
void scheduleALAP(vector<Instruction*> BB, std::map<const Instruction *, int> &schedule, int ASAPFinishTime) {
	// Iterate over instructions in reverse
	schedule = {};
	for (auto it = BB.rbegin(), end = BB.rend(); it != end; it++) {
		// FIXME , do something else here
		int late = 0;
		for (Instruction* I : (*it)->getUsers()) {

				late = std::max(late, schedule[I]);
		}
		schedule[*it] = late + (*it)->getInstructionLatency();
		//it->dump();
	}
	for (auto it = BB.rbegin(), end = BB.rend(); it != end; it++) {
		schedule[*it] = ASAPFinishTime - schedule[*it];
	}

	for (auto it = BB.begin(), end = BB.end(); it != end; it++) {
		cout << "the schedule of " << (*it)->getOperator() <<" is "<<schedule[*it]<< endl;
	}
		
}

/*********************LIST Scheduing***************************/


/*********************Main Function***************************/
int main()
{
	/*user Input*/
	
	string inputop;
	getline(cin, inputop);

	for (size_t i = 0; i < inputop.length(); i++) {
		cout << inputop[i];
	}
	cout << endl;

	//Create a cycle map for the input instruction
	vector<Instruction*> instblock = InstructionParse(inputop);
	cout << endl;

	// Initialize scheduling
	for (i = 0; i < instblock.size(); i++) {
		ASAPschedule.insert(std::pair<Instruction*, int>(instblock[i], 0));
	}

	// Display the mapping relations of each operator
	/*for (i = 0; i < instblock.size(); i++) {
		vector<Instruction*> op_users = (*instblock[i]).getUsers();
		if (op_users.size() != 0) {
			for (j = 0; j < op_users.size(); j++) {
				cout << (*instblock[i]).getOperator() << " user is " << (*op_users[j]).getOperator() << endl;
			}
		}
		else {
			cout << (*instblock[i]).getOperator() << " user is null" << endl;
		}
	}
	for (i = 0; i < instblock.size(); i++) {
		vector<Instruction*> op_pars = (*instblock[i]).getParent();
		if (op_pars.size() != 0) {
			for (j = 0; j < op_pars.size(); j++) {
				cout << (*instblock[i]).getOperator() << " parent is " << (*op_pars[j]).getOperator() << endl;
			}
		}
		else {
			cout << (*instblock[i]).getOperator() << " parent is null" << endl;
		}
	}*/


	// Start ASAP schedule
	cout << "Start ASAP scheduling" << endl;
	ASAPLatency = scheduleASAP(instblock, ASAPschedule);
	cout << "the latency is " << ASAPLatency << endl;
	cout << endl;

	// Start ALAP schedule
	cout << "Start ALAP scheduling" << endl;
	scheduleALAP(instblock, ALAPschedule,ASAPLatency);
	cout << "the latency is " << ASAPLatency << endl;

	
	
	while (1);

    return 0;
}

