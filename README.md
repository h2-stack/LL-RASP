# Low-level RASP: Protecting Applications Implemented in High-level Programming Languages

This is a technology I shared at BHUSA 2023 that can obtain high-level programming language(HPL) context in the native process space. Taking the RASP technology as a reference, I  developed a new technology called  Low-level RASP. I hope to inspire more possibilities and look forward to communicating with you.

You can download the presentation slides by visiting https://i.blackhat.com/BH-US-23/Presentations/US-23-LI-Low-Level-RASP.pdf,Many non-core details are not reflected in the keynote due to time reasons, so I will also share the original manuscript here:

Hi,

I’m Zhuonan Li, The topic of my talk today is Low-level RASP: Protecting Applications Implemented in High-level Programming Languages.

In the last 2 years, I focused on application security from a low-level perspective in order to provide a unified security solution for applications in different programming languages.

For this reason, we prefix this technology with low-level.

The talk will be divided into several sections.

I will first introduce the role of RASP in the process of emergency response.

Next, I'll expose some real-world scenarios that motivate us to find a new technology.

Then here comes the two core sections of this talk: what we expect from this new technology and how to design and implement it.

There is a demo in the next section to make this technology easier to understand.

Finally, I will disclose some metrics and quantification about this technology.

Let’s take Log4Shell as an example, Attack requests are mixed with a large number of normal requests.

Web Application Firewall is a gateway-layer defense technology, it will intercept most attacks because it can see the raw traffic, But there may be some variants OR combinations of variants for the payload because the middleware used by the application may have specific decoding logic, which makes it difficult for WAF to ensure that all the attack requests will be intercepted.

RASP is a defense technology used to address vulnerabilities in the application layer, Requests that bypass gateway-layer defenses will be transferred to this layer. It can get more context, including the raw traffic, the Java layer methods have been executed and the final action has been triggered by this request. With these contexts, RASP can then decide whether an intercept should be taken or not, and that’s the reason why it usually has a better effect than WAF in this situation.

In case RASP has been bypassed for some reason, the mission of defense will fall to HIPS, which is in charge of host layer. However, the information HIPS can get is very limited, such as what the process is going to do and the process chain. For example, if an attacker transfter sensitive data through DNS tunnel, HIPS cannot decide whether it is a offensive behavior or not since it only know that the application is communicating with a public DNS server.

We can see that the advantage of RASP compared with HIPS and WAF is it can get the context information inside the application, that's why RASP usually plays a important role in the emergency response of application layer vulnerabilities.

But in fact, RASP is not enough to protect your applications, Next, I will share some scenarios we have encountered before.

Scene 1: RASP it's not always effective.

Here we use Java Command Execution as an example, Most RASPs set their hook points at Runtime dot getRuntime dot exec(iɡˈzek)

But if we explore the implementation of this method much deeper, we will find that Runtime will eventually call a native method named forkAndExec, which will then call the Java_forkAndExec inside libjvm through Foreign Function Call.

Generally there are 3 methods to bypass a defense software.

The first method is to break the execution flow. It means that we can destroy the original execution flow of RASP through reflection or by retransform the byte code of defense software. For example, the attacker could turn off RASP by setting its key fields through reflect. or, the attacker could retransform the byte codes through Instrument, which will all break the defense logic of RASP, thus bypass it.

The second method is to break the data flow. It means that we can break the rule-check stage of RASP by modifying the data required by the analyzer module. For example, the attacker could forge the original stack trace by starting a new thread. or the attacker could modify the memory areas of rules directly by using Unsafe, this will all make the rule check stage failed, thus bypass it.

The third method is to exploit to the blind zone of defense software. It means that we can do something outside of the control scope of RASP. For example, the attacker could call forkAndExec directly through reflection to bypass the byte code layer Hook Points, or the attacker could call a native method directly through FFI, attacker can do almost anything bad without being intercepted.

Scene 2: The core advantage of RASP is it has the ability to obtain HPL-layer stackTrace, However, RASP has a poor performance when getting the stack trace.

Let's dive into it, We use JVMTI-based RASP as an example, RASP gets the Java layer stack trace by calling Thread dot currentThread dot getStackTrace, which will then calls a native method dumpThreads, then a JNI call to JVM_DumpThreads in native space, However, the FFI call is much slower than function call inside native space.

Besides, when we get stack trace by calling HPL-layer methods, they will return all frame’s stack traces, but in fact, we usually don't need them all. (eg. If you try to get the stack trace of a RCE vulnerability triggered from the springboot framework, you will get dozens or even hundreds of layers of stack trace because there may be many filters registered at the starting stage of the application, but maybe top 10 frames are enough for us to decide whether an interception should be taken or not).

So there are 2 potential performance improvements:

The first point is we can try to get the HPL-layer stack trace from native space directly.

The second point is we can try to get the HPL-layer stack trace of frames in custom range.

Scene 3: In most cases at enterprise, different business teams may choose different programming language according to their own business characteristics.

There are applications implemented in various languages, no matter they running inside different containers or as processes on the same host.

The diversity of programming languages creates greater challenges for security teams, because there are too many things that need to be done to enable RASP in these situations, Firstly, different languages require different hook techniques, For example, Java corresponds to JVMTI, Node.js corresponds to SIGUSR1, We may need to spend a lot of time and human resources to achieve only such a small stage, not to mention the possible operating cost caused by different deployment methods and maintenance costs caused by different implementations.

In short, most security teams cannot accept the cost of implementing RASP separately for each programming languages.

We have listed a few scenes we encountered before, to solve these problems, we need to look up a new technology, and we think this technology should meet the following requirements at least.

Firstly, the technology should have better defense effects, because that is the value of defense software.

Secondly, the technology should have the ability to secure applications in different programming languages with a lower cost, that's the breakthrough of which.

Lastly, the technology should be able to be deployed at a large scale, because defensive software is valuable only when they have been deployed.

Let's explore them in more detail one by one.

The first point, to have a better defense effects, the hook points should be set as low as possible to reduce the possibility of being bypassed, besides, we need this technology to have the ability to get HPL-layer stack trace from native space.

Firstly, the more secure execution flow, Since LL-RASP is working at the native space, there is currently no way for Java to modify the implementation of JVM. Therefore, the execution logic in native space is more secure than in the byte code of Java layer.

Secondly, the more secure data flow, Because we will have better performance when getting stack trace, we can image things we were afraid to do before. For example, full-stack matching, which works by matching stack traces layer by layer within a custom range, besides, we have the ability to take over all memory-related functions at native space, such as the native implementation Unsafe.

Lastly, dimensionality defense, In the implementation model of programming languages, all HPL-layer command execution will eventually be executed through FFI at native space, Any JNI operations such as NativeLibrary dot load can be observed inside JVM, but not vice versa.

This means the defenders will have a better positional advantage than the attackers.

The second point is to secure applications in different programming languages. We think to achieve this goal, the language independent things should be unified and the language dependent part should be as simple as possible.

Let's look at the independent part first, we consider the following functions should be independent with specific language.

Firstly, the Hook module is used to modify the execution logic of specific functions, Here we may choose InlineHook or GOTHook according to the linking method of the target binary.

Secondly, the Rule Module is used to manage security rules for a specific process, such as fetching rules from the server and updating rules to process…

Thirdly, the Analyzer Module is used to decide whether an action is needed according to the event's context and security rules. Although different programming languages may require different security rules, we still consider the implementation of this module should be language-independent.

Lastly, the Control Module is used to receive and execute instructions from the daemon process, for example, install and uninstall probes.

Now we look at the right side, the language dependent part, We think this part should be designed as simple as possible. So far, there is only 2 functions need to implement in this part, one is to generate the HPL-layer stack trace from native space, and the other is to define custom hook points for a specific language.

Finally, we implemented the independent part in librs_engine dot so and the dependent part in librs_lang dot so

The last point is features needed by large-scale, In order to land more agile and effective than ever before, we think the new technology should at least have better compatibility, stability, performance, and lower landing cost.

Firstly, compatibility, We hope to support applications as many as possible, no matter what JDK or Tomcat version the application uses. To achieve this goal, we use ptrace to inject the lightweight extension into the process, and the defense principle of this technology should be independent with user code, framework, middleware, or even the system kernel.

Secondly, stability, this is because there is no business team willing to accept that their business is constantly interrupted by the security team. To achieve this goal, I implemented all related functions without using any library(for example, no libbpf, libelf), which will also avoid the potential supply chain security risks. The second point is memory safe, In order to solve this problem, we use Valgrind to check the lightweight extension, and use Rust to implement the daemon part, Due to the introduction of ownership, Rust is memory-safe by default. The third part is hash verification, we set a restriction that only binaries verified by us can be injected and hooked.

Thirdly, performance, In our case, we use the unix domain socket for IPC, and we have designed a high-performance protocol for RPC. During the attach period of JavaAgent, a process called de-optimization is usually triggered, which may cause the usage of CPU and Memory become very high, this is because the Java layer byte code has been optimized into native code after running for a while. when we trying to retransform or redefine the byte code, these optimized code will be invalidated, and then a large number of recompilations will be required. However in principle, our new technology does not involve JIT for the entire peroid, so this problem does not exist. To improve the performance of getting stack trace, we will get HPL layer stack trace without using FFI, and we can set a custom range of frame, I will explain them in detail later.

Lastly, lower landing cost, defensive software is valuable only when they have been deployed. Here we expect the defense software could be deployed easily, including install and uninstall, We also hope to have the ability to easily update security features, and the prerequirements should be as few as possible.

On this page, we mainly show the structure diagram and attack flow of this technology from internal perspective of the application.

RASP protects applications by retransforming the byte code in Java layer, while LL-RASP secures applications through hook functions inside native space.

Let's look at the attack flow chart on the right side, when an attacker triggers a command execution through various vulnerabilities, a method named forkAndExec will be called eventually. This method will then call Java_xx_forkAndExec in libjvm through FFI, because Java_xx_forkAndExec has been hooked by us before, the call will be redirected to execute our predesigned logic, in librs_jdk we will get the stack trace of HPL-layer, and then pass the context to librs_engine, here the analyzer module will decide whether to take an interception or not according to the contexts and security rules.

Here is a diagram showing the overall architecture, Before introducing this picture, let me first mention another technology that needs to be understood, Universal Defense System, UDS is an extended detection and response software developed also by me and it plays the role of a daemon here.

Logically, UDS is divided into four parts, environment monitor module, security module, status monitor module, and the communication module. LL-RASP is a component of the Security Module, the other three are JavaAgent, used to handle Non-FFI calls inside the Java layer; eBPF, used to handle some kernel-layer functions and NetFilter is used to protect applications with legacy kernel that eBPF can not support.

Suppose there are several applications implemented with Java, Node.js, PHP, Python, and Ruby.

In this environment, we need to run UDS as a daemon, What we need to pay attention to here is the security module, we won't share the other three security components in detail because our focus today is LL-RASP, When a security engineer issues a protect command, UDS will download the language independent library librs_engine dot so and language dependent library librs_lang dot so from the server through RPC, then the engine dot so will be injected into the target process for some initializing works, when it's ready, it will then load the corresponding librs_lang dot so according to the process language, which will then trigger the hook stage.

Here we assume that everyone has the basic acknowledge about inlinehook and gothook, and we will not talk about them.

Now let’s enter the most core part of this talk, how to implement the lightweight extension?

In fact, to implement the lightweight extension for a specific language, we only need to implement two functions, one is to obtain the HPL-layer stack trace from native space, another is to customize the hook points.

As we mentioned in Scene 2 before, the implementation logic used to get Java-layer stack trace is in JVM_DumpThreads, which means we are able to get the HPL-layer stack trace if we can call the method directly from native space.

Let's take Java as the first example, we first obtain the current thread object through JVM_CurrentThread, then we pass the result to JVM_DumpThreads, finally, we convert the jstring into a readable UTF string through GetStringUTFChars, then we could pass the Java layer stack trace to the Analyzer module, which will then determine whether an interception is required or not according to the contexts and security rules.

The second feature we need to implement is to define the custom hook points.

Generally, we need to consider several points when selecting hook points.

Firstly, the hook point should be convergent, which means we should choose the hook point as low as possible to reduce the possibility of being bypassed.

Secondly, the hook point should be stable and fixed, that is to say, we should try to choose points that do not change with language version to improve the compatibility and stability.

It must be noted that there are also other factors need to be considered in combination with the actual situation.

For demonstration purposes, I will list a few typical hook points later.

Since we known that the command execution of Java will eventually call Java_xx_forkAndExec inside libjvm or libjava, where we could set the first hook point.

In order to prevent attackers from bypassing RASP through FFI, we also need to set a hook point to take over the logic of loading dynamic libraries. For Java, both System dot load and System dot loadLibrary will eventually call a native method named NativeLibraries_load inside the native space. So we can set it as the second hook point.

Let's take Node.js as our second example,

The first feature we need to implement is to get the Node.js layer stack trace from native space.

Here we use the node_api header file to achieve this goal, we can get an isolate by calling a method named GetCurrent inside the native space, and then we pass the isolate to CurrentStackTrace to get a StackTrace object, then we can get a custom range of frames by calling GetFrame. For any frame within this range, we can get the line number, script name and function name by calling GetLineNumber, getScripeName and GetFunctionName. Finally, we can assemble this data into a readable string and pass it to the Analyzer module.

The second feature we need to implement is to define custom hook points for Node.js, For demonstration purposes, here we set the hook point at the uv_spawn method in libnode. In fact, the hook point can be any library or executable binary file.

The third example is for PHP.

We use the php header file to get the PHP layer stack trace from the native space, Specifically, we use zend_fetch_debug_backtrace to get the zval object, then we convert it into a zend_array and finally, we assemble this data into a readable string and pass it to the Analyzer module.

The second feature is to define custom hooks for PHP, For demonstration purposes, here we set the hook point at the php_exec inside the executable binary of current process.

This example is for Python.

The first thing need to do is get the Python layer stack trace from native space.

Here we use the Python header file to achieve this goal, Specifically, we use PyThreadState_Get to get the PyThreadState object, from which we can get a PyFrameObject, and then we could pass the frame to PyCode_Addr2Line to get the line number, file name and function name. Finally, we could assemble this data into a readable string and pass it to the Analyzer module.

The second feature is to define custom hooks for Python, For demonstration purposes, here we set the hook point at the system method inside the libpython

The last example is for Ruby.

We use the ruby header file to get the Ruby layer stack trace from native space, Specifically, we use rb_make_backtrace to get the VALUE object, and then we assemble this VALUE into a readable string by using rb_ary_join and rb_string_value_cstr. Finally, we pass the stack trace of the Ruby layer and parameters of this method to the Analyzer module inside engine dot so, which will then determine whether an interception is required or not according to the contexts and security rules.

For demonstration purposes, we set the hook point at the rb_execarg_new method inside the libruby

For demonstration purposes, I have prepared three vulnerable applications that we can trigger command execution by visiting these services directly.

We first demonstrate the Java application

1. Here is the source code

2. Start the service

3. We need to run UDS as a daemon first.

4. Now we can see the information of the current container.

5. Protect

6. The target function has been hooked.

7. Next, visit the Java service in attacker's container to trigger a command execution.

8. The content has been printed.

9. Since the default action is upload when there is no rules, we can see the stack trace at server side.

Next, we demonstrate Node.

1. Since the hash value of the binary file have been verified, we can protect it without any restrictions.

2. The target funtion has been hooked.

3. Trigger command execution.

4. The parameters and stack trace has been upload to the server.

The last demo is Python.

1. Here we use flask as an example

2. Protect

3. The target function has been hooked

4. Trigger command execution.

5. Parameters has been printed

6. The more detailed stack trace is here.

Now, Let's look at some actual effects.

How much work it would take to implement a defensive software for a new language using traditional techniques? We leave this question to everyone, let's take a look at the result of our new technology.

The HPL-independent parts, such as the hook module, the rule module, the analyzer module, and the control module will no longer need to be redesigned and rewritten again since we've implemented the general logic part uniformly before.

In fact, to secure a new HPL, we only need to implement 2 functions, one is to generate the HPL-layer stack trace from native space, another is to define custom hook points. specifically, we only need to write a few dozen lines of code, whereas using other techniques may require hundreds of times the effort to accomplish the same thing.

In 2022, we have verified more than 600 binaries of different programming languages including Java, Node.js, PHP, and Python.

This technology has been deployed to applications implemented in Java, Node.js, PHP, and Python.

Lastly, it has been running stably for a year with 0 failures.

There are three points in the takeaways section.

RASP can block many real-world attacks, but only for applications implemented in specific language.

Most security teams cannot accept the development, deployment, maintenance, and operational costs of implementing RASP for each language individually.

LL-RASP has the advantages of both HIPS and RASP while avoids the disadvantages of each, and it can enable security teams to secure applications more agilely and effectively than ever before.

Finally, If you have any questions, please feel free to contact me.

Thanks.
