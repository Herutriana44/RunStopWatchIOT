import 'dart:async';
import 'package:flutter/material.dart';
import 'package:http/http.dart' as http;
import 'dart:convert';

void main() {
  runApp(MyApp());
}

class MyApp extends StatelessWidget {
  const MyApp({super.key});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'Stopwatch App',
      theme: ThemeData(
        primarySwatch: Colors.blue,
      ),
      home: const StopwatchPage(),
    );
  }
}

class StopwatchPage extends StatefulWidget {
  const StopwatchPage({super.key});

  @override
  _StopwatchPageState createState() => _StopwatchPageState();
}

class _StopwatchPageState extends State<StopwatchPage> {
  final Stopwatch _stopwatch = Stopwatch();
  Timer? _timer;
  String _displayTime = "00:00:00.000";

  @override
  void initState() {
    super.initState();
    _startListeningToFirebase();
  }

  Future<void> _startListeningToFirebase() async {
    const String startUrl =
        '';
    const String finishUrl =
        '';

    while (true) {
      try {
        final startResponse = await http.get(Uri.parse(startUrl));
        final finishResponse = await http.get(Uri.parse(finishUrl));

        if (startResponse.statusCode == 200 &&
            finishResponse.statusCode == 200) {
          final startData = json.decode(startResponse.body);
          final finishData = json.decode(finishResponse.body);

          int start = startData is int ? startData : 0;
          int finish = finishData is int ? finishData : 0;

          if (start == 1 && finish == 0 && !_stopwatch.isRunning) {
            _startStopwatch();
          } else if ((start == 1 && finish == 1) && _stopwatch.isRunning) {
            _stopStopwatch();
          }
        }
      } catch (e) {
        print('Error fetching data: $e');
      }

      await Future.delayed(const Duration(seconds: 1));
    }
  }

  Future<void> _resetStopwatch() async {
    const String baseUrl =
        '';
    const String authQuery = '';

    _stopwatch.reset();
    _timer?.cancel();
    setState(() {
      _displayTime = "00:00:00.000";
    });

    try {
      // Mengirimkan nilai reset (reset, start, dan finish = 0) ke Firebase
      final responseFinnish = await http.put(
        Uri.parse('$baseUrl/finish.json$authQuery'),
        body: json.encode(0),
      );
      final responseStart = await http.put(
        Uri.parse('$baseUrl/start.json$authQuery'),
        body: json.encode(0),
      );
      final responseReset = await http.put(
        Uri.parse('$baseUrl/reset.json$authQuery'),
        body: json.encode(1),
      );

      if (responseFinnish.statusCode == 200 &&
          responseStart.statusCode == 200 &&
          responseReset.statusCode == 200) {
        print('Reset, start, and finish values successfully sent to Firebase');
      } else {
        print('Failed to send reset values to Firebase');
      }
    } catch (e) {
      print('Error sending reset data: $e');
    }
  }

  void _startStopwatch() {
    _stopwatch.start();
    _timer = Timer.periodic(const Duration(milliseconds: 30), (timer) {
      setState(() {
        _displayTime = _formatElapsedTime(_stopwatch.elapsedMilliseconds);
      });
    });
  }

  void _stopStopwatch() {
    _stopwatch.stop();
    _timer?.cancel();
    setState(() {
      _displayTime = _formatElapsedTime(_stopwatch.elapsedMilliseconds);
    });
  }

  String _formatElapsedTime(int milliseconds) {
    int hours = (milliseconds ~/ (1000 * 60 * 60)) % 24;
    int minutes = (milliseconds ~/ (1000 * 60)) % 60;
    int seconds = (milliseconds ~/ 1000) % 60;
    int milliSeconds = milliseconds % 1000;

    return "${hours.toString().padLeft(2, '0')}:"
        "${minutes.toString().padLeft(2, '0')}:"
        "${seconds.toString().padLeft(2, '0')}."
        "${milliSeconds.toString().padLeft(3, '0')}";
  }

  @override
  void dispose() {
    _timer?.cancel();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text('Stopwatch App'),
      ),
      body: Center(
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children: [
            Text(
              _displayTime,
              style: const TextStyle(fontSize: 48),
            ),
            const SizedBox(height: 20),
            ElevatedButton(
              onPressed: _resetStopwatch,
              child: const Text('Reset'),
            ),
          ],
        ),
      ),
    );
  }
}
