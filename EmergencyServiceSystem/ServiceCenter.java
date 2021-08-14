
public class ServiceCenter {
    private WaitingList awl;

    public ServiceCenter() {
        this.awl = new WaitingList();
    }

    /**
     * Record patient's data and add the patient into waiting list
     * @param name
     * @param phoneNumber
     * @param triageLevel
     * @param location
     */
    public void addPatientIntoList(String name, String phoneNumber, int triageLevel, String location) {
        if(0 < triageLevel && triageLevel < 6)
        {
        	Patient patient = new Patient(name, phoneNumber, triageLevel, location);
            if(this.awl.isInList(patient)){
                System.out.println(patient.getName() + " is in waiting list. ");
            }else{
                this.awl.addToList(patient);
                System.out.println("Add " + patient.getName() + " into waiting list. ");
            }
        } else {
        	System.out.println("Sorry, please enter again! ");
        }

    }

    /**
     * Pop out the first patient in the waiting list and assign an Ambulance for him/her
     * @return the patient object
     */
    public Patient assignAmbulanceForPatient() {
        // TODO: The tester report that the system will crash when waiting list is empty
    	if(awl.numNodes != 0) {
            Patient patient = this.awl.popPatient();
            System.out.println("Assigned an ambulance for patient: " + patient.getName());
            return patient;
    	} else {
    		System.out.println("Error! The waiting list is empty.");
    		return null;
    	}
    }
    //this method is for option 3
    public Patient assignAmbulanceForPatientById(int id) {
    	Patient patient = this.awl.popPatientById(id);
    	System.out.println("Success! An ambulance as assigned for patient " + patient.getId());
    	return patient;
    }
    //this method is for option4
    public Patient checkPosition() {
    	Patient patient = this.awl.check();
    	System.out.println(" There are "+ awl.numNodes +" patients before patient " + patient.getId());
    	return patient;
    }

    /**
     * Print out the waiting list
     */
    public void printWaitingList(){
        this.awl.printList();
    }
}