int GetSize(char *buffer){
    int i;
    int size = 0;
    while(buffer[i] != '\0'){
        i++;
        size++;
    }
    return size;
}

void encrypt1(char *buffer){
    int size = GetSize(buffer);

    int j = 0;
    while(j < size){ 
        if(buffer[j] >= 'A' && buffer[j] <= 'Z'){
            buffer[j] = 'A' + (buffer[j] - 'A' + 2) % 26;
        }else if(buffer[j] >= 'a' && buffer[j] <= 'z'){
            buffer[j] = 'a' + (buffer[j] - 'a' + 2) % 26;
        }else if(buffer[j] >= '0' && buffer[j] <= '9'){
            buffer[j] = '0' + (buffer[j] - '0' + 2) % 26;
        }
        j++;
    }
    buffer[size] = '\0';
}

void decrypt1(char *buffer){
    int size = GetSize(buffer);

    int j = 0;
    while(j < size){ 
        if(buffer[j] >= 'A' && buffer[j] <= 'Z'){
            buffer[j] = 'A' + (buffer[j] - 'A' - 2) % 26;
        }else if(buffer[j] >= 'a' && buffer[j] <= 'z'){
            buffer[j] = 'a' + (buffer[j] - 'a' - 2) % 26;
        }else if(buffer[j] >= '0' && buffer[j] <= '9'){
            buffer[j] = '0' + (buffer[j] - '0' - 2) % 26;
        }
        j++;
    }
    buffer[size] = '\0';
}

void reverseWord(int i, int j, char *buffer){
    // Swap
    while(i <= j){
        int temp = buffer[i];
        buffer[i] = buffer[j];
        buffer[j] = temp;
        i++;
        j--;
    }
}

void encryDecry2(char *buffer)
{
    int size = GetSize(buffer);
    for (int i = 0; i < size; i++)
    {
        if (buffer[i] != ' ')
        {
            int j;
            for (j = i; j < size && buffer[j] != ' '; j++)
            {
                // Iterating to the space between 2 words.
            }
            j--;
            int end = j;
            reverseWord(i, end, buffer);
            i = end;
        }
        i++;
    }
}

void encryptData(char *buffer, int policy)
{
    // int policy = atoi(argv[1]);
    if (policy == 0)
    {
        return;
    }
    else if (policy == 1)
    {
        encrypt1(buffer);
    }
    else if (policy == 2)
    {
        encryDecry2(buffer);
    }
}

void decryptData(char *buffer, int policy)
{
    if (policy == 0)
    {
        return;
    }
    else if (policy == 1)
    {
        decrypt1(buffer);
    }
    else if (policy == 2)
    {
        encryDecry2(buffer);
    }
}