
public class WaitingList {
    protected Node head;
    protected int numNodes;

    public WaitingList(){
        this.head = null;
        this.numNodes = 0;
    }
    // this method is for option3
    public Patient popPatientById(int id) {
    	Node tmp = this.head;
    	Node t = null;
    	Node pre = null;
    	if(this.head == null) {
    		System.out.println("waiting list is empty!");
    		return null;
    	} else if (this.head.getPatient().getId() == id) {
    		this.head = this.head.getNext();
    		return tmp.getPatient();
    	} else {
    		while (tmp != null && tmp.getPatient().getId() != id) {
    			pre = tmp;
    			tmp = tmp.getNext();
    		}
    		if(tmp != null) {
    			t = pre.getNext();
    			pre.setNext(tmp.getNext());
    			return t.getPatient();
    		} else {
    			System.out.println("A");
    			return null;
    		}
    	}
    }
    //this method is for option 4
    public Patient check() {
    	Node tmp = this.head;
    	System.out.println(numNodes);
//    	Node t = null;
    	if(this.head == null) {
    		System.out.println("waiting list is empty!");
    		return null;
    	} else {
    		while(tmp.getNext() != null) {
    			tmp = tmp.getNext();
    		}
    		return tmp.getPatient();
    	}

    }


    /**
     * This method will pop the first patient from the waiting list
     * @return the patient popped from the waiting list
     */
    //TODO: Implement this method
    //this method is for option2
    public Patient popPatient() {
    	Node tmp = this.head;
    	Node t = null;
    	if (tmp == null) {
    		System.out.println("waiting list is empty!");
    		return null;
          } else if (tmp.getNext() == null){
        	this.head = null;
        	numNodes--;
        	System.out.println(numNodes);
        	return tmp.getPatient();
          } else {
        	  while(tmp.getNext().getNext() != null) {
        		  tmp = tmp.getNext();
        	  }
        	  t = tmp.getNext();
        	  tmp.setNext(null);
        	  numNodes--;	//calculate the patients number
        	  System.out.println(numNodes);
        	  return t.getPatient();
          }
    }

    /**
     * This method will add patient into the waiting list according to the triage level
     * @param patient patient's data
     */
  //TODO: Implement this method
    //this method is for option 1
    public void addToList(Patient patient) {
    	Node n = new Node(patient);
    	if(this.head == null) {
    		this.head = n;
    		numNodes++;
    	} else {

    		Node tmp = this.head;
    		while(tmp.getNext() != null && n.getPatient().getTriageLevel() > tmp.getNext().getPatient().getTriageLevel()) {
    			tmp = tmp.getNext();
    		}

    		if(tmp.getNext() != null) {
    			n.setNext(tmp.getNext());
    		}
    		tmp.setNext(n);
    		numNodes++;
    	}
    }

    /**
     * print out the information for each patient in waiting list
     */
  //TODO: Implement this method
    //this method is for print the waiting list
    public void printList() {
    	Node tmp = this.head;
    	System.out.println("ID" +"\tNAME" + "\tTriage level");
    	while (tmp != null) {
    		System.out.print(tmp.getPatient().getId());
            System.out.print("\t"+tmp.getPatient().getName());
            System.out.print("\t"+tmp.getPatient().getTriageLevel());
            System.out.println();
            tmp = tmp.getNext();
//            System.out.println("---------------------");
    	}
    }



    /**
     * Check whether the patient is in this list or not
     * @return
     */
    public boolean isInList(Patient patient) {
        if (this.head == null) {
            return false;
        }

        Node temp = this.head;
        while(temp != null) {
            if(temp.getPatient().getName().equals(patient.getName())
                    && temp.getPatient().getPhoneNumber().equals(patient.getPhoneNumber())){
                return true;
            }
            temp = temp.getNext();
        }
        return false;
    }
}