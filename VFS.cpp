#include<iostream>
#include<cstdlib>
#include<iomanip>
#include <stdexcept>
#include<ctime>
#include<fstream>

using namespace std;


class inode;

class PositionList
	{
		public:
			inode* head; // pointer to the head of PositionList
			PositionList (); // empty list constructor
			~PositionList (); // destructor to clean up all nodes
			bool empty() const; // checks if list is empty
			void add(int size, string name, string date, bool type, inode* parent); // add a new Node at the front of the list
			void removeFront(); // remove front Node from front of the list
			inode* remove(string n); //remove a node with name given
			void display();			//displays all items in list
			inode* find(string n);  //finds item with given name
	};

class inode						//class for the base nodes in the tree
{
	public:
		int size;				//stores size
		string name;			//stores name
		string date;			//stores date in dd-mm-yy format
		inode* next;			//points to next item in list
		inode* parent;			//points to parent
		bool type;				//type 0 is a directory and tpe 1 is a file
		PositionList childL; 	//list of pointers to children nodes
		inode(int size, string name, string date, bool type, inode* parent) : size(size), name(name), date(date), type(type), parent(parent), next(NULL) {} //constructor
};

// constructor
PositionList::PositionList ()
{
	this->head = NULL;
}
//====================================
// destructor to clean up all nodes
PositionList::~PositionList () 
{
	
	while (!empty())
		removeFront();
}
//====================================
// Check if the list is empty or not
bool PositionList::empty() const 
{
	
	return this->head == NULL;

}

//====================================
// add a node at the front of list
void PositionList::add(int size, string name, string date, bool type, inode* parent)
{
	inode* newelem= new inode(size,name,date,type,parent);
	newelem->next=this->head;
	this->head=newelem;

}
//====================================
// remove a node from the list
inode* PositionList::remove(string n)
{

	inode* curr=head;
	bool flag=0;
	if (curr->name==n)
	{	inode* prev=this->head;
		this->head=prev->next;
		flag=1;
		return prev;}
	while(curr->next!=NULL)
	{	if (curr->next->name==n)
		{	inode* x= curr->next;
			curr->next=x->next;
			flag=1;
			return x;
			}
		curr=curr->next;}
	if (flag==0)
		return nullptr;
	}

inode* PositionList::find(string n)
{
	inode* temp=head;
	while(temp!=NULL)
	{	if (temp->name==n)
			return temp;
		temp=temp->next;}
	return nullptr;
}

void PositionList::display() 	//displays all items in given format
{	inode*curr=head;
	while(curr!=NULL)
	{	if (curr->type==0)
			cout<<"dir "<<curr->name<<" "<<curr->date<<" "<<curr->size<<"Bytes"<<endl;
		else
			cout<<"file "<<curr->name<<" "<<curr->date<<" "<<curr->size<<"Bytes"<<endl;
		curr=curr->next;}
}

void PositionList::removeFront()
{	if (head!=NULL)
		{inode* prev= this->head;
		this->head=prev->next;
		delete prev;}
}
		
class VFS  													//main tree class
{	
	
	class PathStack											//nested class (stack of strings data strcutre) used to calculate the path
	{public:
		int i=0;
		string arr[100];
		bool empty();
		string top(); 
		void push(string elem);
		void pop();
		int size();
		string path();
		void makeEmpty();};

	
	class Trash												//nested class (queue of inode pointers data strcutre) used to maintain the recycle bin of size 10
	{
		private:
			inode **array;
			int n;	//number of current elements in the Queue
			int f;	//index of front element
			int r;	//index where a new element will be added
		public:
			Trash()
				{
				array= new inode*[10];
				n=0;
				f=0;
				r=0;}

			~Trash()
				{delete [] array;}
			void enqueue(inode*); 
			void dequeue();
			bool isEmpty();
			bool isFull();
			void clean();
			inode* lastelem();
	};

	
public:
	Trash bin;				
	PathStack storer;
	inode* root;			//pointers to store location of precious directory, current directory and root directory
	inode* curr;
	inode* prev;
	VFS(string txt);		//constructor which initialises the tree by reading from a file or initialises an empty tree if file is incorrectly formatted or doesnt exist)
	~VFS();					//destructor which deletes the tree and stores it in a file
	void removeChild(string name);				//method to remove an inode and store it in the bin
	void addChild(int size, string name, bool type, inode* parent); //method to create a new file/folder
	void moveChild(string name, string location);		//method to move a file/folder form one location to another
	void list();	//lists all files and folders under the current directory
	void cdfold(string name); 	//changes current directory to given directory which is inside the current directory
	void cdpar();				//changes current directory to parent directory
	void cdprev();				//changes current directory to previous directory
	void cdroot();				//changes current directory to root directory
	void cdpath(string path);	//changes current directory to a directory at a particular path
	void pwd();					//prints path of current directory
	void find(string name, inode* start);  //prints paths of all files/folders with given name
	string realpath(string name,inode* pointer);	//returns path of any file/folder with given name which is child of given pointer
	void emptybin();			//empties the bin
	void showbin();				//shows the last element in the bin(next element to be deleted)
	void recover();				//recovers the last element in the bin
	int size(string name, inode* x); //returns total size of a file/folder
	string datetostr();				//converts present system date to a string
	inode* interpretpath(string path);	//returns pointers to current node given a path string
	void writer(inode* x);	//recursive function to help the destrcutor in writing to file when program closes

};

VFS::VFS(string txt)			//reads from file and creates the tree. incorrect formatting is caught and new tree will be created if so deemed. ony possible issue here is that if vfs.dat is so written, a file might also have children. this can be skipped easily using another if/else however i didnt do so because it will not cause any error and i couldnt decide whether to consider this a formatting error and delete the whole tree or if that item was to just be skipped
{	ifstream fin;   
	fin.open(txt); 
	if (fin)
	{try
		{string input="";
		string path, parameters;
		int size;
		string date;
		while(input=="")
			getline(fin,input);
		parameters=input.substr(input.find(",")+1);
		size=stoi(parameters.substr(0,parameters.find(",")));
		date=parameters.substr(parameters.find(",")+1);
		root=new inode(size,"/",date,0,NULL);
		while(!fin.eof())
		{	getline(fin,input);
			inode* reader= root;
			if(input.size()!=0)
			{path=input.substr(1,input.find(",")-1);
			parameters=input.substr(input.find(",")+1);
			size=stoi(parameters.substr(0,parameters.find(",")));
			date=parameters.substr(parameters.find(",")+1);
			while(1)
			{	if (path.find("/")==string::npos)
				{	inode* temp=reader;
					if (path.find(".")==string::npos)
						reader->childL.add(size,path,date,0,reader);
					else
						reader->childL.add(size,path,date,1,reader);

					break;}
				else
					{reader=reader->childL.find(path.substr(0,path.find("/")));
					path=path.substr(path.find("/")+1);}

			}
		}}}
		catch(exception &e)
		{cout<<"Exception: "<<e.what()<<endl;
		cout<<"File could not be read due to formatting error- new file will be created"<<endl;
		root= new inode(0,"/",datetostr(),0,NULL);}
		fin.close();}
	else
		root= new inode(0,"/",datetostr(),0,NULL);
	curr=root;
	prev=root;
}
	
void VFS::removeChild(string name)			//checks to see if given string is a name or path. removes directory or file from parent and adds to bin
{
if (name.find("/")==string::npos)
{	if(curr->childL.find(name)!=nullptr)
	{	inode* x=curr->childL.remove(name);
		bin.enqueue(x);}
	else
		cerr<<"File not found"<<endl;}
else
{	inode* node= interpretpath(name);
	if(node==root||node==NULL)
		cerr<<"Invalid path"<<endl;
	else
	{	string name=node->name;
		node=node->parent;
		inode* x= node->childL.remove(name);
		bin.enqueue(x);
		}
}
}


void VFS::addChild(int size, string name, bool type, inode* parent) //adds file/directory to current directory. checks also for duplicates
{	string date=datetostr();
	if(curr->childL.find(name)==nullptr)
	{if (type==0)
		curr->childL.add(10,name,date,0,curr);
	else
		curr->childL.add(size,name,date,1,curr);}
	else
		cerr<<"Duplicate names not allowed within a directory"<<endl;}

void VFS::moveChild(string name, string location)			//checks to see if given name and location strings are paths or name. uses interpret on paths. checks also to see if the moved file/folder won't be a duplicate in folder it is being moved to
{	if (name.find("/")==string::npos && location.find("/")==string::npos)
	{	inode* temp=curr->childL.find(name);
		inode* mover=curr->childL.find(location);
		if (temp==nullptr || mover==nullptr )
			cerr<<"No file/folder with given name found"<<endl;
		else if (mover->type==1)
			cerr<<"Cannot move to a file"<<endl;
		else
		{	if(mover->childL.find(name)==nullptr)
			{temp=curr->childL.remove(name);
			temp->next=mover->childL.head;
			mover->childL.head=temp;
			temp->parent=mover;}
			else
			cerr<<"Destination folder already has an object with name of file to move"<<endl;}
		}
	else if(name.find("/")!=string::npos && location.find("/")!=string::npos)
	{	
		inode* temp=interpretpath(name);
		inode* mover=interpretpath(location);
		if (temp==nullptr || mover==nullptr )
			cerr<<"No file/folder with given name found"<<endl;
		else if (mover->type==1)
			cerr<<"Cannot move to a file"<<endl;
		else
		{	if(mover->childL.find(temp->name)==nullptr)
			{inode* x= temp->parent;
			temp=x->childL.remove(temp->name);
			temp->next=mover->childL.head;
			mover->childL.head=temp;
			temp->parent=mover;}
			else
			cerr<<"Destination folder already has an object with name of file to move"<<endl;}
		}

}

void VFS::list()
{curr->childL.display();}

void VFS::cdroot()
{	prev=curr;
	curr=root;}

void VFS::cdpar()
{	if(curr!=root)
	{prev=curr;
	curr=curr->parent;}
	else
	cerr<<"Cannot go to parent of root"<<endl;}	

void VFS::cdprev()
{	inode* swap=curr;
	curr=prev;
	prev=swap;}

void VFS::cdpath(string path)
{	inode* temp=interpretpath(path);
	if (temp==nullptr)
		cerr<<"Path doesn't exist"<<endl;	
	else if(temp->name.find(".")!=string::npos)	
		cerr<<"Cannot change directory to file"<<endl;
	else
	{	prev=curr;
		curr=temp;}

}
void VFS::cdfold(string name)
{	inode* temp=curr->childL.find(name);
	if (temp==nullptr)
		cerr<<"No folder found"<<endl;
	else if (temp->type==1)
		cerr<<"Cannot change directory to file"<<endl;
	else
	{	prev=curr;
		curr=temp;
	}
}
void VFS::pwd()			//prints working directory by repeatedly pushing the name of parent to stack and then repeatedly popping into string
{	inode* temp=curr;
	if (temp->parent==NULL)
		cout<<"/"<<endl;
	else
	{	while(temp->parent!=NULL)
		{
			storer.push(temp->name);
			temp=temp->parent;
		}
		cout<<storer.path()<<endl;
	}
}
void VFS::find(string name,inode* start)	//recursively searches through tree to find names of folders/file with given name
{	inode* traverse= start;
	if (traverse->type==1)
	{	if (traverse->name==name)
			cout<<realpath(name,start->parent)<<endl;}
	else
	{	if (traverse->name==name)
			cout<<realpath(name,start->parent)<<endl;
		inode* iter=start->childL.head;
		while(iter!=NULL)
		{	find(name,iter);
			iter=iter->next;}
	}
}
string VFS::realpath(string name,inode* pointer)
{	inode* temp=pointer;
	string s="";
	if (temp->parent==NULL)
		s="";
	else
	{	while(temp->parent!=NULL)
		{
			storer.push(temp->name);
			temp=temp->parent;
		}
		s=storer.path();
	}
	s=s+"/"+name;
	return s;
}

void VFS::emptybin()
{bin.clean();}

void VFS::showbin()
{	if (bin.isEmpty())
		cout<<"The bin is empty"<<endl;
	else
	{	cout<<"Next element to remove: "<<realpath(bin.lastelem()->name,bin.lastelem()->parent)<<" ( "<<bin.lastelem()->size<<", "<<bin.lastelem()->date<<" )"<<endl;}}
	
void VFS::recover()			//recovers and adds back file/folder if path still exists. error checking also to check if recovering will duplicate file in the folder
{	if (!bin.isEmpty())
		{inode* recoverback=bin.lastelem();
		string recovery=realpath(recoverback->name,recoverback->parent);
		recovery=recovery.substr(0,recovery.rfind("/"));
		inode* recoverhere=interpretpath(recovery);
		if(recoverhere==nullptr)
			cerr<<"Path doesn't exist"<<endl;
		else if(recoverhere->childL.find(recoverback->name)!=nullptr)
			cerr<<"Cannot recover as path already has file/folder with same name"<<endl;
		else
		{	recoverback->next=recoverhere->childL.head;
			recoverhere->childL.head=recoverback;
			recoverback->parent=recoverhere;
			bin.dequeue();}}
	else
		cerr<<"The bin is empty"<<endl;
}
void VFS::writer(inode* x)	//recusrively appends data of inodes to given file
{	inode* traverse= x;
	ofstream fout;
	fout.open("vfs.dat", ios::out | ios::app);
	if (traverse->type==1)
	{	if (traverse!=root)
			{fout<<realpath(traverse->name,traverse->parent)<<","<<traverse->size<<","<<traverse->date<<endl;
			fout.close();}}
	else
	{	if (traverse!=root)
			{fout<<realpath(traverse->name,traverse->parent)<<","<<traverse->size<<","<<traverse->date<<endl;
			fout.close();}
		inode* iter=x->childL.head;
		while(iter!=NULL)
		{	writer(iter);
			iter=iter->next;}
	}
}
VFS::~VFS()		//opens file, adds record of root node and closes it, calls writer function
{	ofstream fout;
	fout.open("vfs.dat");
	inode* x=root;
	fout<<"/,"<<root->size<<","<<root->date<<endl;
	fout.close();
	writer(x);
	
	
}



int VFS::size(string name, inode* x)	//recursively checks and adds up sizes
{inode* temp;
if(name.find("/")==string::npos)
	temp =x->childL.find(name);
else
	temp =interpretpath(name);

int s=0;
if (temp==nullptr)
{cerr<<"Item not found"<<endl;
return -1;}
else if (temp->type==1)
	return temp->size;
else
{inode* iter= temp->childL.head;
while(iter!=NULL)
{s+=(size(iter->name,temp));
iter=iter->next;}
s+=temp->size;
return s;}

}

string VFS::datetostr()			//takes current system date and formats it to return a string in dd-mm-yy format
{	
	time_t now=time(0);
	tm *ltm = localtime(&now);
	string dayx;
	string monthx;
	string year=to_string(1900 + ltm->tm_year);
	int day=ltm->tm_mday;
	if (day<10)
		dayx="0"+to_string(day);
	else
	    dayx=to_string(day);
	int month=1 + ltm->tm_mon;
	if (month<10)
	    monthx="0"+to_string(month);
	else
	    monthx=to_string(month);
	
	string date=dayx+"-"+monthx+"-"+year;
	return date;
	}

inode* VFS::interpretpath(string path)			//interprets a path string and returns a pointer pointing to it
{	inode* reader= root;
	if (path==""||path=="/")
		return root;
	path=path.substr(1);
	while(1)
			{	if (path.find("/")==string::npos)
				{	return reader->childL.find(path);

					break;}
				else
					{reader=reader->childL.find(path.substr(0,path.find("/")));
					path=path.substr(path.find("/")+1);}
				if (reader==nullptr)
					return nullptr;

			}
}
//methods of nested PathStack class
void VFS::PathStack::makeEmpty()
	{
		i=0;
	}
	
string VFS::PathStack::path()		//empties stack and concatenates to a string
	{
		string path="";
		int size=i;
		while(!empty())
		{	path+=top();
			pop();}
		return path;
	}
bool VFS::PathStack::empty()		//checks to see if stack is empty
	{

		return i == 0;

	}

string VFS::PathStack::top()		//return top element(irrelevant)
	{
			return arr[i-1];
	}
	
void VFS::PathStack::push(string elem)	//pushes an item to the stack
	{

		i+=2;
		arr[i-2]=elem;
		arr[i-1]="/";

	}

void VFS::PathStack::pop()		//pops an item from the stack
	{
			i-=1;
	}

int VFS::PathStack::size()		//returns size(irrelevant)
	{

		return i;
	}


//methods of nested Trash class
void VFS::Trash::enqueue(inode *a)  //adds a pointer to a file/folder to bin
	{if (!isFull())
		{array[r]=a;
		n+=1;
		r=(r+1)%10;}
	else
		{dequeue();
		enqueue(a);}}

void VFS::Trash::dequeue()			//deletes an item from bin
	{
		if(!isEmpty())
		{	f=(f+1)%10;
			n-=1;}
		else
			cerr<<"Bin is empty"<<endl;
	}

bool VFS::Trash::isFull()			//checks to see if bin is full
	{return n==10;}

bool VFS::Trash::isEmpty()
	{return n==0;}

void VFS::Trash::clean()			// removes all items from bin
	{while(!isEmpty())
		dequeue();}

inode* VFS::Trash::lastelem()		//shows last item in bin(added earliest)
	{
		inode* last=array[f];
		return last;
	}
void listCommands() 			//function to list all available commands
{
	cout<<endl<<"List of available Commands:"<<endl<<endl
		<<"Note: mv and rm can be provided with paths also (Bonus questions done: recover,mv,rm)"<<endl<<endl
		<<"help                     : Print list of commands"<<endl
		<<"pwd                      : Prints the path of current directory or file"<<endl
		<<"realpath <filename>      : Prints full path of a given file"<<endl
		<<"ls                       : Lists all contents of current folder"<<endl
		<<"mkdir <folder>           : Create a new folder"<<endl
		<<"touch <file>             : Create a new file"<<endl
		<<"cd <folder>              : Change current directory to a given folder inside it"<<endl
		<<"cd ..                    : Change current directory to parent directory"<<endl
		<<"cd -                     : Change current directory to previous directory"<<endl
		<<"cd                       : Changes current directory to root"<<endl
		<<"cd <path>      	         : Changes current directory to directory at specified path if it exists"<<endl
		<<"find <file/folder>       : Find all paths of files/folders with a given name"<<endl
		<<"mv <file> <folder>       : Move a given file into another folder"<<endl
		<<"rm <folder/file>         : Remove the file or folder and put in the the bin"<<endl
		<<"size <file/folder>       : Print total size of a folder or file"<<endl
		<<"emptybin                 : Empties the bin"<<endl
		<<"showbin                  : Shows the oldest item in the bin"<<endl
		<<"recover                  : Bring back the oldest deleted file or folder"<<endl
		<<"exit                     : Exit the Program"<<endl<<endl<<endl;
}


int main()				//main function 
{

	
	VFS FileSys("vfs.dat");		//initialises our tree
	listCommands();				//lists all commands for starting screen
	string user_input;			
	string sub;
	string command;
	string parameter1;
	string parameter2;

	do
	{
		cout<<">";
		getline(cin,user_input);

		command = user_input.substr(0,user_input.find(" "));
		sub = user_input.substr(user_input.find(" ")+1);
		parameter1=sub.substr(0,sub.find(" "));
		parameter2 = sub.substr(sub.find(" ")+1);		
		try
		{
			if(command=="size") 				 cout<<FileSys.size(parameter1,FileSys.curr)<<" bytes"<<endl;
			else if(command=="pwd")		         FileSys.pwd();
			else if(command=="realpath")		 cout<<FileSys.realpath(parameter1,FileSys.curr)<<endl;

			else if(command=="ls")				FileSys.list();

			else if(command=="mkdir")			//error checking to see if name is alphanumeric
			{	int i=0;
				bool flag=0;
				while(i<parameter1.length())
				{	if(!isalnum(parameter1[i]))
						flag=1;
					i++;}
				if (flag==0)
					FileSys.addChild(10, parameter1, 0, FileSys.curr);
				else
					cerr<<"Directory name must be alphanumeric"<<endl;}
			else if(command=="touch")	     	//error checking to see presence of extension and if name is alphanumeric
			{	int k=0;
				bool flagnum=0;
				while(k<parameter2.length())
				{if (!isdigit(parameter2[k]))
					flagnum=1;
				k++;}
				if(flagnum==1)
					cerr<<"Size must be an integer"<<endl;
				else
				{if(parameter1.find(".")!=string::npos)
				{	int i=0;
					int j=0;
					bool flag=0;
					string s=parameter1.substr(0,parameter1.find(".")-1);
					string f=parameter1.substr(parameter1.find(".")+1);

					while(i<s.length())
					{	if(!isalnum(s[i]))
							flag=1;
						i++;}
					while(j<f.length())
					{	if(!isalnum(f[j]))
							flag=1;
						j++;}
					if (flag==0)
						FileSys.addChild(stoi(parameter2), parameter1, 1, FileSys.curr);
					else
						cerr<<"File name must be alphanumeric apart from extension dot"<<endl;}
				else
					cout<<"Files must have an extension"<<endl;}}

			else if(command=="cd")
			{	if (user_input=="cd"||user_input=="cd ")
					FileSys.cdroot();
				else if(parameter1=="..")
					FileSys.cdpar();
				else if(parameter1=="-")
					FileSys.cdprev();
				else if(parameter1.find("/")==string::npos)
					FileSys.cdfold(parameter1);
				else
					FileSys.cdpath(parameter1);
			}
			else if(command=="find")            FileSys.find(parameter1,FileSys.root);
			else if(command=="mv")	        	FileSys.moveChild(parameter1, parameter2);

			else if(command =="help")			listCommands();
			else if(command=="emptybin")        FileSys.emptybin();
			else if(command=="showbin")			FileSys.showbin();
			else if(command == "recover")		FileSys.recover();
			else if(command == "rm")			FileSys.removeChild(parameter1);
			else if(command == "exit")			break;
			else 								cout<<"Invalid Command !!"<<endl;
		}
		catch(exception &e)
		{
			cout<<"Exception: "<<e.what()<<endl;
		}

	}while(command!="exit" and command!="quit");

	return EXIT_SUCCESS;
}



