import 'package:flutter/material.dart';
import 'package:get/get.dart';
import 'package:starpy/home_controller.dart';

void main() async {
  Get.lazyPut(() => HomeController());
  runApp(
    const GetMaterialApp(
      debugShowCheckedModeBanner: false,
      title: "Starflut Example",
      home: HomeView(),
    ),
  );
}

class HomeView extends GetView<HomeController> {
  const HomeView({Key? key}) : super(key: key);
  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text('Starflut'),
        centerTitle: true,
      ),
      body: Center(
        child: ElevatedButton(
            onPressed: controller.runPythonCode,
            child: const Text("Run Python")),
      ),
    );
  }
}
