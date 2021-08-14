/*==================================
Foundations of Computer Science
Student: Qianxun Huai
id: a1813515
Semester:1
Year:2020
Practical Exam Number:05
===================================*/

public class Queue {
	//required properties
	Node back;
	protected int count = 0;

	//basic constructor
	public Node getBack() {
		return back;
	}

	//parametirc constructor
	public void setBack(Node back) {
		this.back = back;
	}




	//required method
	public void enqueue(Student tmpStudent) {
		Node nodeStudent = new Node(tmpStudent);
		nodeStudent.next = this.back;
		this.back = nodeStudent;
		count++;
	}

	//required method
	public void dequeue() {
		if(this.back == null) {
			System.out.println("Can't remove, queue is empty");
		} else if (this.back.next == null) {
			this.back = null;
			count--;
		} else {
			Node tmp = this.back;
			while(tmp.next.next != null) {
				tmp = tmp.next;
			}
			tmp.next = null;
			count--;
		}
	}
	//used to count the number
	private int count(Node count) {
		int calculate = 1;
		while(count.next != null) {
			count = count.next;
			calculate++;
		}
		return calculate;
	}
	//display queue
	public void displayQueue() {
		if(this.getBack() == null) {
			// System.out.println("Sorry, queue is empty");
		} else {
			print();
		}
	}
	//print the queue
	private void print() {
		Node tmp = this.getBack();
		Node count = this.getBack();

		while(tmp != null) {
			int num = count(count);
			if(tmp.getInfo().getPeriod() == 1) {
				System.out.println("#"+num+" " +tmp.getInfo().getName()+", " +tmp.getInfo().getAge() +" years old, "
						+tmp.getInfo().getPeriod()+ "st year in Hogwarts;");
			} else if (tmp.getInfo().getPeriod() == 2) {
				System.out.println("#"+num+" " +tmp.getInfo().getName()+", " +tmp.getInfo().getAge() +" years old, "
						+tmp.getInfo().getPeriod()+ "nd year in Hogwarts;");
			} else if(tmp.getInfo().getPeriod() == 3) {
				System.out.println("#"+num+" " +tmp.getInfo().getName()+", " +tmp.getInfo().getAge() +" years old, "
						+tmp.getInfo().getPeriod()+ "rd year in Hogwarts;");
			} else {
				System.out.println("#"+num+" " +tmp.getInfo().getName()+", " +tmp.getInfo().getAge() +" years old, "
						+tmp.getInfo().getPeriod()+ "th year in Hogwarts;");
			}
			tmp = tmp.next;
			count = count.next;
		}
	}

}
