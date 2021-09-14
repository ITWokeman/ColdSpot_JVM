public class ThreadTest extends Thread {
 
	private static int i = 1;

	public ThreadTest(String name){
		super(name);
	}
	
	public static void main(String[] args) {
		new ThreadTest("黄牛A").start();
		new ThreadTest("黄牛B").start();
	}
 
	public void run() {
		for(;i < 200;i++) {
			//当通过继承Thread类的方式实现多线程时，可以直接使用this获取当前执行的线程
			System.out.println(Thread.currentThread().getName()+"抢到第"+i+"张票");
		}
		synchronized(ThreadTest.class){
			System.out.println(1);
		}
	}

}