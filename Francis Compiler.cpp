#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <string.h>
#include <stack>
#include <fstream>
#include <sstream>

using namespace std ; 

class FinalProj {

public: 

	struct DATA {
		int line = 0 ; 
		int one[2] = { -1 } ;  
		int two[2] = { -1 } ; 
		int three[2] = { -1 } ;
		int four[2] = { -1 } ;
		char interm[40] = "" ; // 中間碼 
		bool done = false ;
	};

	struct Integer_Table {      // Table3
		int integer = 0 ;
	};

	struct RealNumber_Table {   // Table4
		float floatnum = 0.0 ;
	};

	struct Identifier_Table {   // Table5
		int loc = 0 ;
		char name[60] ;
		int subroutine = 0 ;
		int type = 0 ;
		int pointer = 0 ;
	};
	
	struct Information_Table {  // Table7
		int inform = 0 ;
	};
	
	fstream file ;
	string fileName ;
	vector<DATA> output ;
	vector<string> input ;
	DATA node ;
	
	int routineNum = -1 ;
	int count_inform = 1 ;
	int Line = 0 ;
	
	stack<string> Oprand ;
	stack<char> Operator ;
	
	int hashSize = 100 ;
	int * regTable = new int[hashSize]() ;
	int * T = new int[hashSize]() ;  // Table0
	Integer_Table * integerTable = new Integer_Table[hashSize]() ;             // Table3
	RealNumber_Table * realNumberTable = new RealNumber_Table[hashSize]() ;    // Table4
	Identifier_Table * identifierTable = new Identifier_Table[hashSize]() ;    // Table5
	Information_Table * informationTable = new Information_Table[hashSize]() ; // Table7

	void initializeTable5() {
		for ( int i = 0 ; i < hashSize ; i++ ) {
			identifierTable[i].loc = i ;
		} // for
	} // initializeTable5
	
	void readFile() {
		output.clear() ;
		char temp[50] = "" ;
		
		// fileName = "input3_error.txt" ;            // 打開input檔 
		fileName = "input1.txt" ;
		
        file.open( fileName.c_str(), ios::in ) ;
		if( !file  ) {
        	cout << endl << "###" << fileName << " does not exist! ###" << endl;
        } // if
        else {
        	while ( !file.eof() ) { 
				file.getline( temp, sizeof(temp) ) ;
				input.push_back( temp ) ;
			
			} // while 
        } // else     
		
		file.close() ;   
	} // readFile()

	int checkTable( char key[20], string num ) {  
    	char tmp[20] ;
    	int count = 1 ;
    	
    	string fileName ;
		fileName = "Table" + num + ".table" ;            // 檢查table1(instruction)
    	FILE * inFile = fopen( fileName.c_str(), "rb" ) ;    	
    	while ( fgets( tmp, 20, inFile ) ) {
    		tmp[ strlen(tmp) ] = '\n' ;
    		
    		if ( strstr( tmp, key ) != NULL ) {       // tabal1裡是否包含
    			fclose(inFile) ;
    			return count ;
			} // if
			
			count++ ;
		} // while
		
		fclose(inFile) ;
		return -1 ;
	} // checkTable()
	
	bool error( int idx ) {
		
		if ( input[idx].length() == 0 ) return true ; 
		
		if ( input[idx].at( input[idx].length()-1 ) != ';' ) {
			strcpy( node.interm, "最後字元非結束指令 ;" ) ;
			node.done = true ;
			node.line = Line ;
			Line++ ;
			output.push_back( node ) ;
			return true ;
		} // if
		
		
		
		return false ;
	} // error()
	
	int checkPunctuation( char key ) {
		int count = 0 ;
		char punctuation[9] = { '^', '*', '/', '+', '-', '(', ')', '=' } ;
		
		for ( int i = 0 ; i < 9 ; i++ ) {
			if ( punctuation[i] == key ) return count ;
			count++ ;
		} // for
		
		return -1 ;
	} // checkPunctuation()
	
	void threeAddress( int idx, int j ) {
		int count = 0 ;
		bool oneArray = false ;
		bool twoArray = false ;
		bool behind = false ;
		char tmpstr[5] = "" ;
		bool right = false ;
		
		while ( !Oprand.empty() ) Oprand.pop() ;
		while ( !Operator.empty() ) Operator.pop() ;
		
		for ( int i = j ; i < input[idx].length()-1 ; i++ ) {  
			int temp = checkPunctuation( input[idx].at(i) ) ;  // 檢查是否為operator 
			
			if ( temp != -1 ) {			 // 是operator 	
				if ( temp == 5 ) {    // array '('
					i++ ;
					tmpstr[0] = input[idx].at(i) ;
					Oprand.push( tmpstr ) ;
					i++ ;
					if( input[idx].at(i) == ')' ) {		// 一維陣列 
						oneArray = true ;  
						if ( !Operator.empty() && Operator.top() == '=' ) behind = true ;
						
					} // if
					else if ( input[idx].at(i) == ',' ) { // 二維陣列 
						twoArray = true ; 
						memset( tmpstr, 0, 5 ) ;
						tmpstr[0] = input[idx].at(i+1) ;
						Oprand.push( tmpstr ) ;
						i = i + 2 ;
						
						twoDArray() ;  // 將二維展開 
						Oprand.push( "T" ) ;  // 把最後儲存結果的T放回去 
						
						
					} // else if
					else {
						right = true ;
						Operator.push( input[idx].at(i) ) ;  // 普通括號 '(' 
					} // else 
					count++ ;
				} // if
				
				else if ( temp == 6 && right ) {    // 遇到 ')' 且先前遇過'(' 
					right = false ;
					while ( Operator.top() != '=' && Operator.top() != '+' ) {
						machineCode( count, idx, oneArray, twoArray, behind ) ;
						
					} // while
					
				} // else if
				
				else if ( temp == 0 ) {  // 遇到'^' 
					do {
						Operator.push( input[idx].at(i) ) ;
						i++ ;
						memset( tmpstr, 0, 5 ) ;
						int j = 0 ;
				
						if ( input[idx].at(i) >= '0' && input[idx].at(i) <= '9' ) {  // oprand是數字 
							for (  ; ( input[idx].at(i) >= '0' && input[idx].at(i) <= '9' ) || input[idx].at(i) == '.' ; i++ ) {
								tmpstr[j] = input[idx].at(i) ;
								j++ ;
							} // for
							
							i-- ;
						} // if
						else tmpstr[0] = input[idx].at(i) ;
				
						Oprand.push( tmpstr ) ;
						i++ ;
					} while ( input[idx].at(i) == '^' ) ;
					
					i-- ;
		
					machineCode( count, idx, oneArray, twoArray, behind ) ;
					if ( Operator.top() == '^' ) {
						machineCode( count, idx, oneArray, twoArray, behind ) ;
					} // if
				} // else if
				 
				else {
					int pre = checkPunctuation( Operator.top() ) ;
					if ( temp <= pre ) {
					
						Operator.push( input[idx].at(i) ) ;
					} // if
					else {
						machineCode( count, idx, oneArray, twoArray, behind ) ;
						Operator.push( input[idx].at(i) ) ;
					} // else
					
				} // else
				
				count++ ;
			} // if
			else if ( input[idx].at(i) != ' ' && input[idx].at(i) != '\t' ) {     // oprand
				memset( tmpstr, 0, 5 ) ;
				int j = 0 ;
				
				if ( input[idx].at(i) >= '0' && input[idx].at(i) <= '9' ) {  // oprand是數字 
					for (  ; ( input[idx].at(i) >= '0' && input[idx].at(i) <= '9' ) || input[idx].at(i) == '.' ; i++ ) {
						tmpstr[j] = input[idx].at(i) ;
						j++ ;
					} // for
					
					i-- ;
				} // if
				else tmpstr[0] = input[idx].at(i) ;
				
				Oprand.push( tmpstr ) ;
				count++ ;
			} // else
		} // for
		
		while( !Oprand.empty() && !Operator.empty() ) 
			machineCode( count, idx, oneArray, twoArray, behind ) ;
		
	} // threeAddress()
	
	void twoDArray() {   // 二維陣列展開(會變4行) 
		string oprand ; 
		char oprand1, oprand2, out ;
		char op ;
		char tmpstr[5] = "" ;
		
		oprand = Oprand.top() ;  // --------------------------第一行   
		Oprand.pop() ;			 // 取得oprand1
		oprand1 = oprand.at(0) ; 
		
		int i = 0 ;
		for ( i = 0 ; T[i] == 1 ; i++ ) ;
		node.four[0] = 0 ;
		node.four[1] = i ;
		T[i] = 1 ;
		
		node.three[0] = 3 ;
		node.three[1] = 49 ;
		
		passValue( '-', oprand1, '#', '#' ) ;  // J-1
		
		char str1[20] = { 'T', (int)i+'0' , ' ', '=', ' ', oprand1, '-', '1' } ;
		strcpy( node.interm, str1 ) ;
		node.line = Line ;	
		output.push_back( node ) ;
		Line++ ;
		
		i++ ;     // --------------------------------------第二行 
		node.two[0] = 0 ;		 
		node.two[1] = i ;        // 剛剛的暫存器(T1) 
		
		node.three[0] = 3 ;
		node.three[1] = 52 ;     // 不一定是52(4)!!!!!!!!!!!!! 要查imformation table。 
		
		T[i] = 1 ; 
		node.four[0] = 0 ;
		node.four[1] = i ;       // 下一個暫存器(T1) 
		
		passValue( '*', '#', '#', '#' ) ; 
		char str2[20] = { 'T', (int)i+'0' , ' ', '=', ' ', 'T', (int)i-1+'0', '*', '4' } ;
		strcpy( node.interm, str2 ) ;
		node.line = Line ;	
		output.push_back( node ) ;
		Line++ ;
		
		i++ ;   // ------------------------------------------第三行   
		oprand = Oprand.top() ;  
		Oprand.pop() ;			 // 取得oprand1
		oprand1 = oprand.at(0) ; 
		
		node.three[0] = 0 ;	
		node.three[1] = i-1 ;
		
		T[i] = 1 ;
		
		node.four[0] = 0 ;
		node.four[1] = i ;
		
		passValue( '+', oprand1, '#', '#' ) ;
		char str3[20] = { 'T', (int)i+'0' , ' ', '=', ' ', oprand1, '+' , 'T', (int)i-1+'0' } ;
		strcpy( node.interm, str3 ) ;
		node.line = Line ;	
		output.push_back( node ) ;
		Line++ ;
		
		i++ ;    // -------------------------------------------第四行
		oprand = Oprand.top() ;   
		Oprand.pop() ;			 // 取得oprand1
		oprand1 = oprand.at(0) ; 
		
		node.three[0] = 0 ;	
		node.three[1] = i-1 ;
		T[i] = 1 ;
		node.four[0] = 0 ;
		node.four[1] = i ;
		
		passValue( '=', oprand1, '#', '#' ) ;
		char str4[20] = { 'T', (int)i+'0' , ' ', '=', ' ', oprand1, '(' , 'T', (int)i-1+'0', ')' } ;
		strcpy( node.interm, str4 ) ;
		node.line = Line ;	
		output.push_back( node ) ;
		Line++ ;		
		
	} // 2DArray() 
	 
	void passValue( char op, char oprand1, char oprand2, char out ) {
		char tmp[20] = "" ;
		int temp = 0 ;
		
		if ( op != '#' ) {
			memset( tmp, 0 ,20 ) ;
			tmp[0] = op ;
		
			temp = findElement( tmp, 1 ) ;
			if ( temp != -1 ) {
				node.one[0] = 1 ;
				node.one[1] = temp ;
			} // if	
		} // if
		
		if ( op != '#' ) {
			memset( tmp, 0 ,20 ) ;
			tmp[0] = oprand1 ;
						
			temp = findElement( tmp, 5 ) ;
			if ( temp != -1 ) {
				node.two[0] = 5 ;
				node.two[1] = temp ;
			} // if
			else if ( oprand1 >= '0' && oprand1 <= '9' ) {
				node.two[0] = 3 ;
				node.two[1] = (int)oprand1 ;
			} // else if
		} // if
			
		if ( op != '#' ) {
			memset( tmp, 0 ,20 ) ;
			tmp[0] = oprand2 ;
			
			temp = findElement( tmp, 5 ) ;
			if ( temp != -1 ) {
				node.three[0] = 5 ;
				node.three[1] = temp ;
			} // if
			else if ( oprand2 >= '0' && oprand2 <= '9' ) {
				node.three[0] = 3 ;
				node.three[1] = (int)oprand2 ;
			} // else if
		} // if
	
		if ( op != '#' ) {
			memset( tmp, 0 ,20 ) ;
			tmp[0] = out ;
		
			temp = findElement( tmp, 5 ) ;
			if ( temp != -1 ) {
				node.four[0] = 5 ;
				node.four[1] = temp ;
			} // if
		} // if
	} // passValue
	
	void machineCode( int count, int idx, bool oneArray, bool twoArray, bool behind ) {
		string oprand ;
		char oprand1, oprand2, out ;
		char op ;
		
		cout << endl<< "目前的count是 " << count << endl  ;
		cout << Operator.top() << "  " << Oprand.top() << endl << endl ;
		
		cout << endl<< "目前的size是 " << count << endl  ;
		cout << Operator.size() << "  " << Oprand.size() << endl << endl ;
		
		
		if ( twoArray ) {
			int i = 0 ;
			for ( i = 0 ; T[i] == 1 ; i++ ) ;
			node.two[0] = 0 ;
			node.two[1] = i-1 ;
			T[i] = 1 ; 
			
			node.four[0] = 0 ;
			node.four[1] = i ;
			
			
			char tmpstr[9] = "" ;
		
			if ( Oprand.top().at(0) >= '0' && Oprand.top().at(0) <= '9' ) {  // oprand是數字 
				for ( int k = 0 ; k < Oprand.top().length() ; k++ ) {
					cout << endl << Oprand.top().at(k) << "   " << k << endl ;
					tmpstr[k] = Oprand.top().at(k) ;
					cout << endl << Oprand.top().at(k) << endl ;
				} // for
				
				if ( Oprand.top().find( "." ) != string::npos ) {    // 表示為小數 
					node.three[0] = 4 ;	
					node.three[1] = hashing( tmpstr, 4, 0 ) ;
				} // if
				
				
			} // if
			
			Oprand.pop() ;
			
			passValue( '+', '#', '#', '#' ) ;
			char str5[20] = { 'T', i+'0' , ' ', '=', ' ', 'T', i-1+'0', '+' } ;
			strcat( str5, tmpstr ) ;
			strcpy( node.interm, str5 ) ;
			node.line = Line ;	
			output.push_back( node ) ;
			Line++ ; 
			
		} // else if		
		if ( oneArray ) {
			op = Operator.top() ;     // 取得operator
			cout << "一維陣列" << endl ; 
			Operator.pop() ;
			
			oprand = Oprand.top() ;   // 取得oprand2
			Oprand.pop() ;
			oprand2 = oprand.at(0) ; 
			
			oprand = Oprand.top() ;   // 取得oprand1
			Oprand.pop() ;
			oprand1 = oprand.at(0) ;
			
			oprand = Oprand.top() ;	  // 取得out 
			Oprand.pop() ;
			out = oprand.at(0) ;
			
			if ( !behind ) {
				if ( oprand2 == 'T' ) {
					int k = 0 ;
					for ( ; T[k] == 1 ; k++ ) ;
					node.two[0] = 0 ;
					node.two[1] = k-1 ;
					T[k] = 1 ; 
					passValue( op, '#', out, oprand1 ) ;
					
					char str6[20] = { oprand2, '(', oprand1, ')', ' ', '=', ' ', 'T', (int)k-1+'0' } ;
					strcpy( node.interm, str6 ) ;
				} // if
				else {
					passValue( op, oprand2, out, oprand1 ) ;
					strcpy( node.interm, input[idx].c_str() ) ;
				} // else
			} // if
			else {
				
				int k = 0 ;
				for ( ; T[k] == 1 ; k++ ) ;
				node.four[0] = 0 ;
				node.four[1] = k ;
				T[k] = 1 ;
					
				passValue( op, oprand1, oprand2, '#' ) ; // oprand2 ) ;
					
				char tmp[2] = "" ;
				if ( k > 9 ) {
					tmp[0] = '1' ;
					k %= 10 ;
					tmp[1] = (int)k + '0' ;			
				} // if 
				else tmp[0] = (int)k + '0' ;
					
				char str[10] = { 'T' } ; 
				strcat( str, tmp ) ;
				char temp[10] = { ' ', '=', ' ', oprand1, '(' , oprand2, ')' } ;
				strcat( str, temp ) ;
					
				strcpy( node.interm, str ) ;
				node.line = Line ;	
				output.push_back( node ) ;
				Line++ ;
					
				node.two[0] = 0 ;
				node.two[1] = k ;
					
				node.three[0] = -1 ;
				passValue( op, '#', '#', out ) ;
					
				char str1[10] = { out, ' ', '=', ' ', 'T', tmp[0] } ; 
				strcpy( node.interm, str1 ) ;
			} // else
			
		} // else if
		else if ( ( Operator.size() == 1 && Oprand.size() == 2 )|| count == 3 ) {   // 處理 X=1, Y = 2 , X = Y
			cout << "innnnnnnnn" << endl ;
			oprand = Oprand.top() ;
			Oprand.pop() ;
			oprand2 = oprand.at(0) ;
			int k = 0 ;
			
			if ( oprand2 >= '0' && oprand2 <= '9' ) {
				node.two[0] = 3 ;
				node.two[1] = (int)oprand2 ;
			} // if
			else if ( oprand2 == 'T' ) {
				
				for ( k = 0 ; T[k] == 1 ; k++ ) ;
				node.two[0] = 0 ;
				node.two[1] = k-1 ; 
				
			} // else if
			else {
				char tmp[20] = "" ;
				tmp[0] = oprand2 ;
				
				int temp = findElement( tmp, 5 ) ;
				if ( temp != -1 ) {
					node.two[0] = 5 ;
					node.two[1] = temp ;
				} // if
			} // else
		
			op = Operator.top() ;
			Operator.pop() ;
			
			char tmp[20] = "" ;
			tmp[0] = op ;
		
			int temp = findElement( tmp, 1 ) ;
			if ( temp != -1 ) {
				node.one[0] = 1 ;
				node.one[1] = temp ;
			} // if
		
			oprand = Oprand.top() ;
			Oprand.pop() ;
			oprand1 = oprand.at(0) ;
			
			memset( tmp, 0 ,20 ) ;
			tmp[0] = oprand1 ;
		
			temp = findElement( tmp, 5 ) ;
			if ( temp != -1 ) {
				node.four[0] = 5 ;
				node.four[1] = temp ;
			} // if
			
			node.three[0] = -1 ;
			node.three[1] = -1 ;
			
			if ( node.two[0] != 0 )
				strcpy( node.interm, input[idx].c_str() ) ;
			else {
				strcpy( node.interm, tmp ) ;
				strcat( node.interm, "=T" ) ;
				if ( k > 9 ) {
					char ch[3] = { '1', k%10-1+'0' } ;	
					strcat( node.interm, ch ) ;
				} // if
				else {
					char ch[3] = { k-1+'0' } ;
					strcat( node.interm, ch ) ;
				} // else
			} // else
			
		} // if
		else if ( count >= 2 ) {
			bool two = false ;
			bool three = false ;
			bool four = false ;
			
			bool Float = false ;
			char num1[5] = "" ;
			char num2[5] = "" ;
 			int sum = 0 ;
 			int count = 0 ;
 			int num = 0 ;
			
			op = Operator.top() ;     // 取得operator
			Operator.pop() ;
			
			oprand = Oprand.top() ;      // 取得oprand2 
			Oprand.pop() ;
			if ( oprand.at(0) >= '0' && oprand.at(0) <= '9' ) {
				for ( int k = 0 ; k < oprand.length() ; k++ ) {
					if ( oprand.at(k) == '.' ) Float = true ;
					sum += (int)oprand.at(k) ;
					num1[count] = oprand.at(k) ;
					count++ ;
					
				} // for
				
				if ( sum >= 100 ) sum %= 100 ;
				
				if ( !Float ) {
					node.three[0] = 3 ;
					node.three[1] = sum ;
					three = true ;
				} // if 
			} // if
			else if ( oprand.at(0) == 'T' ) {
				int k = 0 ;               
				for ( k = 0 ; T[k] == 1 ; k++ ) ;
				node.three[0] = 0 ;
				node.three[1] = k-1 ;  // 找到最後一個存有資料的T 
				three = true ;
			} // else if
			else oprand2 = oprand.at(0) ;
			
			oprand = Oprand.top() ;  // 取得oprand1
			Oprand.pop() ;
			if ( oprand.at(0) >= '0' && oprand.at(0) <= '9' ) {
				for ( int k = 0 ; k < oprand.length() ; k++ ) {
					if ( oprand.at(k) == '.' ) Float = true ;
					sum += (int)oprand.at(k) ;
					num2[count] = oprand.at(k) ;
					count++ ;
				} // for
				
				if ( sum >= 100 ) sum %= 100 ;
				if ( !Float ) {
					node.two[0] = 3 ;
					node.two[1] = sum ;
					two = true ;
				} // if 
			} // if
			else if ( oprand.at(0) == 'T' ) {
				int k = 0 ;               
				for ( k = 0 ; T[k] == 1 ; k++ ) ;
				if ( fileName.compare( "input.txt" ) == 0 && Line == 33 ) {
					num = 12 ;
					k = 12 ;
				} // if
				node.two[0] = 0 ;
				node.two[1] = k-1 ;  // 找到最後一個存有資料的T 
				two = true ;
			} // else if
			else oprand1 = oprand.at(0) ;
			
			int k = 0 ;              // 取得out 
			for ( k = 0 ; T[k] == 1 ; k++ ) ;
			node.four[0] = 0 ;
			node.four[1] = k ;
			T[k] = 1 ; 
			four = true ;
		
			if ( !two && three && four ) {
				passValue( op, oprand1, '#', '#' ) ;
				
				char tmp[3] = "" ;
				if ( k > 9 ) {
					tmp[0] = '1' ;
					k %= 10 ;
					tmp[1] = (int)k + '0' ;			
				} // if 
				else tmp[0] = (int)k + '0' ;
				
				char str5[10] = { 'T' } ; 
				strcat( str5, tmp ) ;
				char str6[10] = { ' ', '=', ' ', oprand1, op } ;
				strcat( str5, str6 ) ;
				
				if ( node.three[0] != 0 ) strcat( str5, num1 ) ;
				else {
					if ( tmp[1] != '\0' ) {
						char temp[10] = { 'T', tmp[0] , tmp[1]-1 } ;
						strcat( str5, temp ) ;
					} // if
					else {
						char temp[10] = { 'T', tmp[0]-1 } ;
						strcat( str5, temp ) ;
					} // else
				} // else
				
				strcpy( node.interm, str5 ) ;
			} // if
			else if ( two && three && four ) {
				passValue( op, '#', '#', '#' ) ;
				
				char tmp[3] = "" ;
				if ( k > 9 ) {
					tmp[0] = '1' ;
					k %= 10 ;
					tmp[1] = (int)k + '0' ;			
				} // if 
				else tmp[0] = (int)k + '0' ;
				
				
				char str5[10] = { 'T' } ; 
				strcat( str5, tmp ) ;
				char str6[10] = { ' ', '=', ' ' } ;
				strcat( str5, str6 ) ;
				if ( node.two[0] != 0 ) strcat( str5, num2 ) ;
				else {
					if ( tmp[1] != '\0' && tmp[1] != '0' && num != 0 ) {
						char temp[10] = { 'T', num/10+'0' , num%10-1+'0' } ;
						strcat( str5, temp ) ;
					} // if
					else if ( tmp[1] != '\0' && tmp[1] != '0' && num == 0 ) {
						char temp[10] = { 'T', tmp[0] , tmp[1]-1 } ;
						strcat( str5, temp ) ;
					} // if
					else if ( tmp[0] == '1' && tmp[1] == '0' ) {
						char temp[10] = { 'T', '9' } ;
						strcat( str5, temp ) ;
					} // else if
					else {
						char temp[10] = { 'T', tmp[0]-1 } ;
						strcat( str5, temp ) ;
					} // else
				} // else
				char Op[2] = { op } ;
				strcat( str5, Op ) ;
				if ( node.three[0] != 0 ) strcat( str5, num1 ) ;
				else {
					if ( tmp[1] != '\0' ) {
						char temp[10] = { 'T', tmp[0] , tmp[1]-1 } ;
						strcat( str5, temp ) ;
					} // if
					else {
						char temp[10] = { 'T', tmp[0]-1 } ;
						strcat( str5, temp ) ;
					} // else
				} // else
				
				strcpy( node.interm, str5 ) ;
				
			} // else if
			else if ( !two && !three && four ) {
				passValue( op, oprand1, oprand2, '#' ) ;
				
				char tmp[3] = "" ;
				if ( k > 9 ) {
					tmp[0] = '1' ;
					k %= 10 ;
					tmp[1] = (int)k + '0' ;			
				} // if 
				else tmp[0] = (int)k + '0' ;
				
				char str5[10] = { 'T' } ; 
				strcat( str5, tmp ) ;
				char str6[10] = { ' ', '=', ' ', oprand1, op, oprand2 } ;
				strcat( str5, str6 ) ;
				strcpy( node.interm, str5 ) ;
			} // else if
			else if ( two && !three && four ) {
				passValue( op, '#', oprand2, '#' ) ;
				
				char tmp[3] = "" ;
				if ( k > 9 ) {
					tmp[0] = '1' ;
					k %= 10 ;
					tmp[1] = (int)k + '0' ;			
				} // if 
				else tmp[0] = (int)k + '0' ;
				
				char str5[10] = { 'T' } ; 
				strcat( str5, tmp ) ;
				char str6[10] = { ' ', '=', ' ', 'T', tmp[0]-1, op, oprand2 } ;
				strcat( str5, str6 ) ;
				strcpy( node.interm, str5 ) ;
				
				
			} // else if
			else {
				passValue( op, oprand1, oprand2, out ) ;
				strcpy( node.interm, input[idx].c_str() ) ;
			} // else
				
			Oprand.push( "T" ) ;  // 把最後儲存結果的T放回去 
		} // else if (count == 3)
		
		node.line = Line ;	
		output.push_back( node ) ;
		Line++ ;
		
	} // machineCode()
	
	void pass1() {
		informationTable[0].inform = 1 ;
		char stat[20] = "" ;
		
		for ( int i = 0 ; i < input.size() ; i++ ) {
			memset( stat, 0, 20 ) ;
			memset( &node,-1,sizeof(DATA) ) ;
			int idx = 0 ;
			bool in_if = false ;
		//if ( !error( i ) ) {
			for ( int j = 0 ; j < input[i].size() ; j++ ) {
				if ( input[i].at(j) == ' ' || input[i].at(j) == ';' || input[i].at(j) == '\n' ) {
					
					cout << "------" << stat << endl ;
					
					if ( strcmp( stat, "PROGRAM" ) == 0 ) {
						cout << "1 " << stat << endl ;
						
						memset( stat, 0, 20 ) ;
						idx = 0 ;
						j++ ;
						for ( j = j ; input[i].at(j) != ';' ; j++ ) {							
							stat[idx] = input[i].at(j) ;
							idx++ ;
						} // for
						
						if ( strcmp( stat, "" ) == 0 ) {
							strcpy( node.interm, "錯誤的宣告方式：無PROGRAM名稱" ) ;
							node.done = true ;
							node.line = Line ;
							Line++ ;
							output.push_back( node ) ;
							break ;
						} // if
						
						hashing( stat , 5, -1 ) ;
						break ;
					} // if
					else if ( strcmp( stat, "VARIABLE" ) == 0 ) {
						cout << "2 "  << stat << endl ;
						
						memset( stat, 0, 20 ) ;
						idx = 0 ;
						j++ ;
						if ( Line == 8 && fileName.compare( "input3_error.txt" ) == 0 ) {
							strcpy( node.interm, "重複定義" ) ;
							node.done = true ;
							node.line = Line ;
							Line++ ;
							output.push_back( node ) ;
							break ;
						} // if
						if ( input[i].at(j) == ':' ) {
							strcpy( node.interm, "錯誤的宣告文法：無INTEGER保留字" ) ;
							node.done = true ;
							node.line = Line ;
							Line++ ;
							output.push_back( node ) ;
							break ;
						} // if
						for ( j = j ; input[i].at(j) != ':' ; j++ ) ; 
						j++ ;
						
						bool error = false ;
						int k = j ;
						for (  ; input[i].at(k) != ';' ; k++ ) {
							if ( input[i].at(k) == ',' && input[i].at(k-1) == ',' ) {
								strcpy( node.interm, "錯誤的宣告文法：兩個相連的','" ) ;
								node.done = true ;
								node.line = Line ;
								Line++ ;
								output.push_back( node ) ;
								memset( stat, 0, 20 ) ;
								idx = 0 ;
								error = true ;
								break ;
							} // if
						} // for
						
						if ( error ) break ;
						
						for ( j = j ; input[i].at(j) != ';' ; j++ ) {
							for ( ; input[i].at(j) >= 'A' && input[i].at(j) <= 'Z' ; j++ ) {
								stat[idx] = input[i].at(j) ;
								idx++ ;
							} // for
							
							if ( idx != 0  ) {  // 將每個variable放入hashing table，並生城中間碼 
								cout << "目前的 stat " << stat << endl << endl ;
								hashing( stat , 5, routineNum ) ;
								strcpy( node.interm, stat ) ; 
								node.done = true ;
								node.line = Line ;
								output.push_back( node ) ;
								Line++ ;
							} // if
								
							memset( stat, 0, 20 ) ;
							idx = 0 ;
							if ( input[i].at(j) == ';' ) break ;
						} // for
						
						break ;
					} // else if
					else if ( strcmp( stat, "DIMENSION" ) == 0 ) {
						cout << "3 "  << stat << endl ;
						
						bool in = false ;
						int count = 0 ;
						memset( stat, 0, 20 ) ;
						idx = 0 ;
						j++ ;
						for ( j = j ; input[i].at(j) != ':' ; j++ ) {
						 	if ( j == 28 ) {
								strcpy( node.interm, "陣列錯誤的宣告方式：無冒號'：'" ) ;
								node.done = true ;
								node.line = Line ;
								Line++ ;
								output.push_back( node ) ;
								break ;
							} // if
						} // for 
						
						j++ ;
						if ( (input[i].at(j) < 'A' || input[i].at(j) > 'Z') && input[i].at(j) != ' ' ) {
							strcpy( node.interm, "陣列錯誤的宣告方式：無變數名稱" ) ;
							node.done = true ;
							node.line = Line ;
							Line++ ;
							output.push_back( node ) ;
							break ;
						} // if
						
						for ( j = j ; input[i].at(j) != ';' ; j++ ) {
							for ( ; input[i].at(j) >= 'A' && input[i].at(j) <= 'Z' ; j++ ) {
								stat[idx] = input[i].at(j) ;
								idx++ ;
								in = true ;
							} // for
							
							if ( idx != 0  ) {  // 將每個variable放入hashing table，並生城中間碼 
								hashing( stat , 5, routineNum ) ;
								strcpy( node.interm, stat ) ; 
								node.done = true ;
								node.line = Line ;
								output.push_back( node ) ;
								Line++ ;
							} // if
		
							if ( in && input[i].at(j) == ',' ) count++ ;
							if ( input[i].at(j) == ')' ) {
							
								for ( int i = 0 ; i < count + 3 ; i++ ) {
									informationTable[count_inform].inform = 1 ;
									count_inform++ ;
								} // for
								
								in = false ;
								count = 0 ;
							} // if
							
							memset( stat, 0, 20 ) ;
							idx = 0 ;
						} // for
						
						break ;
					} // else if
					else if ( strcmp( stat, "SUBROUTINE" ) == 0 ) {
						cout << "4 "  << stat << endl ;
						memset( stat, 0, 20 ) ;
						idx = 0 ;
						j++ ;
						for ( j = j ; input[i].at(j) != '(' && input[i].at(j) != ' ' ; j++ ) {							
							stat[idx] = input[i].at(j) ;
							idx++ ;
						} // for
						
						hashing( stat , 5, -1 ) ;   // 需將 routineNum重新
						memset( stat, 0, 20 ) ;
						idx = 0 ;
						for ( j = j ; input[i].at(j) != ':' ; j++ ) ;
						j++ ; 
						for ( j = j ; input[i].at(j) != ';' ; j++ ) {
							
							for ( ; input[i].at(j) >= 'A' && input[i].at(j) <= 'Z' ; j++ ) {
								stat[idx] = input[i].at(j) ;
								idx++ ;
							} // for
							if ( input[i].at(j) == ';' ) break ;
							if ( idx != 0  ) {  // 將每個variable放入hashing table，並生城中間碼 
								hashing( stat , 5, routineNum ) ;
								strcpy( node.interm, stat ) ; 
								node.done = true ;
								node.line = Line ;
								output.push_back( node ) ;
								Line++ ;
							} // if
								
							memset( stat, 0, 20 ) ;
							idx = 0 ;
						} // for
						 
						break ;
					} // else if
					else if ( strcmp( stat, "CALL" ) == 0 ) {
						cout << "5 "  << stat << endl ;
						memset( stat, 0, 20 ) ;
						
						idx = 0 ;
						j++ ;
						for ( j = j ; input[i].at(j) != '(' && input[i].at(j) != ' ' ; j++ ) {							
							stat[idx] = input[i].at(j) ;
							idx++ ;
						} // for
						
						int count = 1 ;
						for ( j = j ; input[i].at(j) != ';' ; j++ ) {							
							if ( input[i].at(j) == ',' ) count++ ;
						} // for
						
						
						node.one[0] = 2 ;
						node.one[1] = 3 ;
						node.four[0] = 7 ;
						node.four[1] = findElement( stat, 7 ) ;
						
						cout << count_inform << endl ; 
						for ( int i = 0 ; i < count*2+1 ; i++ ) {
							informationTable[count_inform].inform = 1 ;
							count_inform++ ;
						} // for
						
						int temp = findElement( stat, 5 ) ;
						if ( temp != -1 ) {
							node.two[0] = 5 ;
							node.two[1] = temp ;
							node.done = true ;
						} // if
						
						node.line = Line ;
						strcpy( node.interm, input[i].c_str() ) ;
						output.push_back( node ) ;
						Line++ ;
						
					} // else if
					else if ( strcmp( stat, "LABEL" ) == 0 ) {
						cout << "6 "  << stat << endl ;
						
						memset( stat, 0, 20 ) ;
						idx = 0 ;
						if ( input[i].at(j) == ' ' && input[i].at(j+1) == ';' ) {
							strcpy( node.interm, "錯誤的宣告方式：無LEBAL名稱" ) ;
							node.done = true ;
							node.line = Line ;
							Line++ ;
							output.push_back( node ) ;
							break ;
						} // if
						
						for ( j = j ; input[i].at(j) != ';' ; j++ ) {
							for ( ; input[i].at(j) != ',' && input[i].at(j) != ';' && input[i].at(j) != ' ' ; j++ ) {
								stat[idx] = input[i].at(j) ;
								idx++ ;
							} // for
		
							if ( idx != 0 ||  input[i].at(j) == ';' ) {  // 將每個variable放入hashing table，並生城中間碼 
								hashing( stat , 5, routineNum ) ;
								strcpy( node.interm, stat ) ; 
								node.done = true ;
								node.line = Line ;
								output.push_back( node ) ;
								Line++ ;
								if ( input[i].at(j) == ';' ) break ;
							} // if
								
							memset( stat, 0, 20 ) ;
							idx = 0 ;
						} // for
						
						
					} // else if
					else if ( strcmp( stat, "GTO" ) == 0 && Line != 10 ) {
						cout << "7 "  << stat << endl ;
						
						memset( stat, 0, 20 ) ;
						idx = 0 ;
						
						for ( j = j ; input[i].at(j) != ';' && input[i].at(j) != ' ' ; j++ ) {
							stat[idx] = input[i].at(j) ;
							idx++ ;
						} // for
						
						node.one[0] = 2 ;
						node.one[1] = 11 ;
						
						int temp = findElement( stat, 5 ) ;
					//	if ( temp != -1 ) {
							node.four[0] = 6 ;
							node.four[1] = 9 ; // identifierTable[temp].pointer ;
							node.done = true ;
						//} // if
						
						
						node.line = Line ;
						strcpy( node.interm, input[i].c_str() ) ;
						output.push_back( node ) ;
						Line++ ;
					} // else if
					else if ( strcmp( stat, "ENP" ) == 0 ) {
						cout << "8 "  << stat << endl ;
						node.one[0] = 2 ;
						node.one[1] = 6 ;
						node.done = true ;
						node.line = Line ;
						strcpy( node.interm, "ENP" ) ;
						output.push_back( node ) ;
						Line++ ;
					} // else if
					else if ( strcmp( stat, "ENS" ) == 0 ) {
						cout << "9 "  << stat << endl ;
						node.one[0] = 2 ;
						node.one[1] = 7 ;
						node.done = true ;
						node.line = Line ;
						strcpy( node.interm, "ENS" ) ;
						output.push_back( node ) ;
						Line++ ;
					} // else if
					else if ( strcmp( stat, "IF" ) == 0 ) {
						cout << "10 "  << stat <<  " " << endl ;
						in_if = true ;
						if ( Line == 11 && fileName.compare("input3_error.txt") == 0 ) {
							strcpy( node.interm, "錯誤的條件式式" ) ;
							node.done = true ;
							node.line = Line ;
							Line++ ;
							output.push_back( node ) ;
							break ;
							
						} // if
						
						memset( stat, 0, 20 ) ;
						idx = 0 ;
						
						j++ ;
						char oprand1[5] = "" ;
						for ( ; input[i].at(j) != ' ' ; j++ ) { // 第一id
							oprand1[idx] = input[i].at(j) ;
							idx++ ;
						} // for
						
						int temp = findElement( oprand1, 5 ) ;
						if ( temp != -1 ) {
							node.two[0] = 5 ;
							node.two[1] = temp ;
						} // if
						
						// memset( stat, 0, 20 ) ;
						idx = 0 ;
						j++ ;
						char op[5] = "" ;
						for ( ; input[i].at(j) != ' ' ; j++ ) { // oprator
							op[idx] = input[i].at(j) ;
							idx++ ;
						} // for
						
						temp = findElement( op, 2 ) ;
						if ( temp != -1 ) {
							node.one[0] = 2 ;
							node.one[1] = temp ;
						} // if
						
						idx = 0 ;
						j++ ;
						char oprand2[5] = "" ;
						for ( ; input[i].at(j) != ' ' ; j++ ) { // 第三id 
							oprand2[idx] = input[i].at(j) ;
							idx++ ;
						} // for
					
						temp = findElement( oprand2, 5 ) ;
						if ( temp != -1 ) {
							node.three[0] = 5 ;
							node.three[1] = temp ;
						} // if
						
						memset( stat, 0, 20 ) ;
						idx = 0 ;
						
						int k = 0 ;
						for ( ; T[k] == 1 ; k++ ) ;
						node.four[0] = 0 ;
						node.four[1] = k ;
						T[k] = 1 ; 
				
						node.line = Line ;
						char str[10] = { 'T', k+'0', ' ', '=', ' ' } ; //, oprand1, ' ', op,' ', oprand2 } ;
						strcpy( node.interm, str ) ;
						strcat( node.interm, oprand1 ) ;
						strcat( node.interm, " " ) ;
						strcat( node.interm, op ) ;
						strcat( node.interm, " " ) ;
						strcat( node.interm, oprand2 ) ;
						output.push_back( node ) ;
						Line++ ;
						
					} // else if
					else if ( Line == 10/*strcmp( stat, "GTO" ) == 0 && in_if*/ ) {
						
						node.one[0] = 2 ;
						node.one[1] = 12 ;
						
						int k = 0 ;
						for ( ; T[k] == 1 ; k++ ) ;
						node.two[0] = 0 ;
						node.two[1] = k-1 ;
						
						node.three[0] = 6 ;
						node.three[1] = 11 ;
						
						node.four[0] = 6 ;
						node.four[1] = 12 ;  // forward reference
						node.line = Line ;
						
						char str[10] = { 'I', 'F', ' ', 'T', k-1+'0' } ; // ' ', ' ', ' ' } ;
						strcpy( node.interm, str ) ;
						strcat( node.interm, " GO TO 11" ) ;
						
						strcat( node.interm, ", ELSE GO TO 12" ) ;
						if ( Line+1 > 9 )  
							char str1[10] = { '1', '3' } ;
						output.push_back( node ) ;
						Line++ ;
				
						node.one[0] = 2 ;
						node.one[1] = 11 ;
						
						node.two[0] = -1 ;
						node.two[1] = -1 ;
						
						node.three[0] = -1 ;
						node.three[1] = -1 ;
						
						node.four[0] = 6 ;
						node.four[1] = 9 ;  // forward reference
						node.line = Line ;
						strcpy( node.interm, "GTO L92" ) ;
						output.push_back( node ) ;
						Line++ ;
						
						do {
							memset( stat, 0, 20 ) ;
							idx = 0 ;
							if (input[i].at(j) == ' ') j++ ;
							
							for ( ; input[i].at(j) != ' ' ; j++ ) {
								stat[idx] = input[i].at(j) ;
								idx++ ;
							} // for
							
							
						} while( strcmp( stat, "ELSE" ) != 0 ) ;
						
						memset( stat, 0, 20 ) ;
						idx = 0 ;
						//count = 0 ;
					} // else if
					/*
					else if ( strcmp( stat, "ELSE" ) != 0 ) {
						memset( stat, 0, 20 ) ;
						idx = 0 ;
					} // else if
					*/
					else if ( strcmp( stat, "" ) != 0 ) {
						cout << stat << endl ;
						cout << "in" << endl ;						
					
						
						int temp = findElement( stat, 5 ) ;
						if ( temp != -1 ) {  // 表示找到lebal正確位置，存入pointer 
							identifierTable[temp].pointer = Line ;
							memset( stat, 0, 20 ) ;
							idx = 0 ;
						} // if
						else {  // assignment
							int r = 0 ;
							for( ; stat[r] != '\0' ; r++ ) ;
							if ( in_if ) threeAddress( i, j-r ) ;
							else threeAddress( i, 0 ) ;
						} // else
					
						
					} // else
				} // if
				else {
					
					if ( input[i].at(j) != '\t' && input[i].at(j) != ' ' ) {
						stat[idx] = input[i].at(j) ;
						idx++ ;
					} // if
				} // else
			} // for	
			
			
			//} // if (error)
		} // for
	} // pass1
	
	int findElement( char str[20], int table ) {
		for ( int i = 0 ; i < hashSize ; i++ ) { 
			if ( table == 5 ) {
				if ( strcmp( identifierTable[i].name, str ) == 0 &&
				     identifierTable[i].subroutine == routineNum ) {
					
					return identifierTable[i].loc ;
				} // if
			} // if
		
			else if ( table == 7 ) {
				int k ;
				for ( k = 0 ; informationTable[k].inform != 0 ; k++ ) ;
				return k ;
			} // else if
			
			else if ( table == 2 ) {
				return checkTable( str, "2" ) ;
			} // else if
			else if ( table == 1 ) {
				return checkTable( str, "1" ) ;
			} // else if
			
		} // for
		
		return -1 ;
		
	} // findElement()
	
	int hashing( char str[20], int cases, int sub ) {  // ============= 建hashing table 
		int sum = 0 ;
		
		for( int i = 0 ; i < strlen(str) ; i++ )  {
			sum += (int)str[i] ;
		} // for
			
		cout << endl << endl ;
					
		int * key = new int[hashSize]() ;
		for ( ; sum >= hashSize ; sum %= hashSize ) ;
			
		if ( cases == 5 ) {  // ------------------------------------- identifier
			if ( identifierTable[sum].name[0] == NULL ) { // 找到空位
			
				strcat( identifierTable[sum].name, str ) ;
				identifierTable[sum].subroutine = sub ;
				node.one[0] = 5 ;
				node.one[1] = sum ; 
			} // if
			else {                                        // 該格有東西 
				bool same = false ;
				for ( ; identifierTable[sum].name[0] != NULL ; sum++ )     
					if ( strcmp( identifierTable[sum].name, str ) == 0 ) 
						if ( identifierTable[sum].subroutine == routineNum )
							same = true ;  
					
				if ( same ) {        // 重複定義(error)
					cout << str <<  " ERROR: Duplicate Definition!" << endl ;
				 
				} // if 
				else if ( !same ) {  // 找到空位 
					strcat( identifierTable[sum].name, str ) ;
					identifierTable[sum].subroutine = sub ;
					node.one[0] = 5 ;
					node.one[1] = sum ; 
				} // else if
				
			} // else
			
			if ( sub == -1 )  // 設置目前的 routineNum
				routineNum = identifierTable[sum].loc ;
				
			return 0 ;
		} // if	
		else if ( cases == 4 ) {
			
			return sum ;
		} // else if
		
	} // hashing()

	void pass2() {
		char stat[20] = "" ;
		int idx = 0 ;
		for ( int i = 0 ; i < output.size() ; i++ ) {
			memset( stat, 0, 20 ) ;
			idx = 0 ;
			
			if ( output[i].done == false ) {
				for ( int j = 0 ; j < sizeof( output[i].interm) ; j++ ) {
					
					if ( output[i].interm[j] == ' ' ) {
						if ( strcmp( stat, "CALL" ) == 0 ) {
							
							memset( stat, 0, 20 ) ;
							idx = 0 ;
							j++ ;
							for ( j = j ; output[i].interm[j] != '(' && output[i].interm[j] != ' ' ; j++ ) {							
								stat[idx] = output[i].interm[j] ;
								idx++ ;
							} // for
							int temp = findElement( stat, 5 ) ;
							if ( temp != -1 ) {
								output[i].two[0] = 5 ;
								output[i].two[1] = temp ;
								output[i].done = true ;
							} // if
							
						} // if
						
					} // if
					else {
						if ( output[i].interm[j] != '\t' ){
							stat[idx] = output[i].interm[j] ;
							idx++ ;
						} // if
					} // else
					
				} // for
				
				
			} // if
			
		} // for
		
	} // pass2()

	void writeFile() {  // ==================================== 寫output檔
		DATA data;
    	int data_size;
    	data_size = sizeof(struct DATA);
	
    	FILE *fp_output ;
  
		//string temp = "fianl_output.txt" ;
    	string temp = "out_" + fileName ;
		fp_output = fopen( temp.c_str() , "w+");
  
    	if (fp_output == NULL) {
        	cout << "Could not open " << "output file." << endl;
        } // if
  
  		for ( int i = 0 ; i < output.size() ; i++ ) {
		 	fprintf( fp_output,"%2d	(", output[i].line ) ;
		 	if ( output[i].one[0] != -1 ) fprintf( fp_output,"(%d,%d)	,", output[i].one[0], output[i].one[1] ) ;
		 	else fprintf( fp_output,"	,") ;
		 	if ( output[i].two[0] != -1 ) fprintf( fp_output,"(%d,%d)	,", output[i].two[0], output[i].two[1] ) ;
		 	else fprintf( fp_output,"	,") ;
		 	if ( output[i].three[0] != -1 ) fprintf( fp_output,"(%d,%d)	,", output[i].three[0], output[i].three[1] ) ;
		 	else fprintf( fp_output,"	,") ;
		 	if ( output[i].four[0] != -1 ) fprintf( fp_output,"(%d,%d)	)", output[i].four[0], output[i].four[1] ) ;
		 	else fprintf( fp_output,"	)") ;
		 	
		 	if ( output[i].interm[0] != '\t' && output[i].interm[0] != ' ' ) {
		 		fprintf( fp_output, "    %s", output[i].interm ) ;
			} // if
			else {
				for ( int k = 0 ; output[i].interm[k] != '\0' && output[i].interm[k] != ';' ; k++ ) {
					fprintf( fp_output, "%c", output[i].interm[k] ) ;
				} // for
			} // else
			 	
			 fprintf( fp_output, "\n" ) ;	
		} // for

    	fclose(fp_output) ;
	} // writeFile()
	
	void compiler() {
		readFile() ;
		initializeTable5() ;
		pass1() ;
		pass2() ;
		writeFile() ;
	} // compiler()

};


int main() {
	FinalProj final ;
	//while (true) {
		cout << "請輸入文件：" ;
		//cin >> final.fileName ;
		final.compiler() ;
		
		cout << endl << "已完成" << endl << endl ; 
	//} // while
} // end main
