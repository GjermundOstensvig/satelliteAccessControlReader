int convertToInt(byte byteBuffer[18]){
  char textBuffer[16];
  //Convert from bytes to ASCII characters
  for (int i = 0; i < 16; i++){
    textBuffer[i] = (char)byteBuffer[i];
  }
  
  //Convert from char array to long int
  int personId = atol(textBuffer);
  return personId;
}

