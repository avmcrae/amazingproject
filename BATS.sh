#!/bin/bash                                                                              
#                                                                                        # BATS.sh                                                                                
# This is a test script for the amazing_project executable 
#                                                          
#########################################################

AmazingTest="AmazingTestlog.`date +%a_%b_%d_%T_%Y`"


echo "This is a test for amazing_project" > $AmazingTest;
echo "Test began: " `date` >> $AmazingTest;
echo $'\n' >> $AmazingTest;

CLEAN=`make clean`;
ALL=`make all`;

echo "Results of running make clean:" >> $AmazingTest;
echo $CLEAN >> $AmazingTest;
echo $'\n' >> $AmazingTest;

echo "Results of running make all:" >> $AmazingTest;
echo $ALL >> $AmazingTest;
echo $'\n' >> $AmazingTest;

echo "Results of running amazing_project with no arguments (./amazing_project):" >> $AmazingTest;
./amazing_project &>> $AmazingTest;
echo  $'\n' >> $AmazingTest;

echo "Results of running amazing_project with 1 argument:" >> $AmazingTest;
./amazing_project 2 &>> $AmazingTest;
echo $'\n' >> $AmazingTest;

echo "Results of running amazing_project with 3 arguments:" >> $AmazingTest;
./amazing_project a b c &>> $AmazingTest;
echo  $'\n' >> $AmazingTest;

echo "Results of running amazing_project with a bad switch:" >> $AmazingTest;
./amazing_project -a 1 -n 2 -d 3 &>> $AmazingTest;
echo  $'\n' >> $AmazingTest;

echo "Results of running amazing_project with a -n above MAX_NUM_AVATARS" >> $AmazingTest;
./amazing_project -n 12 -d 2 -h pierce.cs.dartmouth.edu &>> $AmazingTest;
echo  $'\n' >> $AmazingTest;

echo "Results of running amazing_project with a -d above MAX_DIFFICULTY" >> $AmazingTest;
./amazing_project -n 3 -d 11 -h pierce.cs.dartmouth.edu &>> $AmazingTest;
echo  $'\n' >> $AmazingTest;

echo "Logfile results for running with 3 avatars on depth 2:" >> $AmazingTest;
./amazing_project -n 3 -d 2 -h pierce.cs.dartmouth.edu >/dev/null;
cat Amazing_Raboso_3_2.log >> $AmazingTest;
echo  $'\n' >> $AmazingTest;



echo "Test complete." >> $AmazingTest;
