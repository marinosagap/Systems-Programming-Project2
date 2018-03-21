#!/bin/bash

flag_command=0
flag_path=0
flag_number=0
size_number=0
for str 
	do 
	if [ "$str" -eq "$str" ] 2>/dev/null; then #koitw thn periptwsh pou dwsei integer sto size [n]
		flag_number=1
		size_number=$str
	fi		

	if [ $str == "-l" ]
		then 
		flag_path=1
		continue;
	fi
	if [ $str == "-c" ]
		then 
		flag_command=1
		continue;
	fi
	if [ $flag_command == '1' ]
		then
		mycommand=$str
		flag_command=0
	fi
	if [ $flag_path == '1' ]
		then
		path=$str
		flag_path=0
	fi
done

dirName=${path-‘pwd‘}
if [ $mycommand == "list" ] #an to command einai list tote ektypwnoume tous katalogous
	then
	for file in  "$( find $dirName -type d )"
	do 
		echo "$file"
	done 
fi

i=0
zero=0
if [ "$flag_number" -eq 0 ] #an den edwse orisma sto size tote to size_nubmer = plithos ypofakelwn
	then 
	size_number=$(find $dirName -type d | wc -l)
fi

if [ $mycommand == "size" ] #an to command einai list tote ektypwnoume tous katalogous
	then
	sorted_list=$(find $dirName -type d | ls -S $dirName  | head -n $size_number ) #| tail -n $size_number )
	for k in $sorted_list
	do
		echo $k
	done
fi 

if [ $mycommand == "purge" ] #an to command einai purge tote diagrafoume tous yfistamenous fakelous tou <path> alla kai ton path
	then
	for file in  "$( find $dirName -type d )"
	do 
		rm -rf $file		
	done
fi