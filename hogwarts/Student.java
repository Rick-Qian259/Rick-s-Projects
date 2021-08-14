/*==================================
Foundations of Computer Science
Student: Qianxun Huai
id: a1813515
Semester:1
Year:2020
Practical Exam Number:05
===================================*/


public class Student {
	//properties
	private String name;
	private int age;
	private int period;

	//accessors and mutators
	public String getName() {
		return name;
	}

	public void setName(String name) {
		this.name = name;
	}

	public int getAge() {
		return age;
	}

	public void setAge(int age) {
		this.age = age;
	}

	public int getPeriod() {
		return period;
	}

	public void setPeriod(int period) {
		this.period = period;
	}

	//basic constructor
	Student(){
		name = "unknown";
		age = 0;
		period = 0;
	}

	//parametric constructor
	Student(String tmpName, int tmpAge, int tmpPeriod){
		name = tmpName;
		age = tmpAge;
		period = tmpPeriod;
	}


}
