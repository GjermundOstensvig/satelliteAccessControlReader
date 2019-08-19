//Connects to database, checks for personId and returns a boolean true if it exists in DB
bool personIdInDatabase(int personId){
  row_values *row = NULL;
  
  //Connect to Database
  Serial.println("Connecting to database");
  if (conn.connect(server_addr, 3306, user, password)) {
    delay(1000);
  }
  else{
    Serial.println("Connection failed.");
    }

  delay(1000);
  // Initiate the query class instance
  MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
  // Execute the query
  cur_mem->execute(query);
  // Fetch the columns required
  column_names *columns = cur_mem->get_columns();
  
  do {
    row = cur_mem->get_next_row();
    //Check if id exists in database.
    if (row != NULL && atol(row->values[0]) == personId) {
      Serial.print(row->values[1]);
      Serial.println(" was found in the database");
      cur.close();
      conn.close();
      return true;
    }
  } while (row != NULL);
  // Deleting the cursor also frees up memory used
  delete cur_mem; 
  cur.close();
  conn.close();
  
  Serial.println("the person was not found in the database");
  return false;
}


