int digit_cnt(char *input){
    int counter=0;    
    int i=0;
    while(input[i]!=0){
        if(input[i]>='0'&&input[i]<='9'){
            counter++;
        }
        i++;
    }
    return counter;
}

int main(int argc, char **argv){
    digit_cnt(argv[1]); 
    return 0;
}