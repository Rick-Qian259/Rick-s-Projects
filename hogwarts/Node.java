/*==================================
Foundations of Computer Science
Student: Qianxun Huai
id: a1813515
Semester:1
Year:2020
Practical Exam Number:05
===================================*/

public class Node {
	protected Node next;
	protected Student info;

	Node(){
		next = null;
	}

	Node(Student tmpStudent){
		info = tmpStudent;
	}

	public Node getNext() {
		return next;
	}

	public void setNext(Node next) {
		this.next = next;
	}

	public Student getInfo() {
		return info;
	}

	public void setInfo(Student info) {
		this.info = info;
	}



}
