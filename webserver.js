const express = require('express');
const path = require('path');
const compression = require('compression');
const bodyParser = require('body-parser');
const assert = require('assert');
const spawn = require('child_process').spawn;
const co = require('co');
const delay = require('./delay.js');

var config = {
  number: "",
  lastWords: [],
  wordsRemaining: 0,
  status: "done",
  opCount: 0,
  decomposed: ""
};

const app = express();

app.use(compression());

app.use( bodyParser.json() );       // to support JSON-encoded bodies
app.use(bodyParser.urlencoded({     // to support URL-encoded bodies
  extended: true
})); 

app.use("/", express.static(__dirname + '/public'));
app.set('view engine', 'ejs'); 

app.get('/', function(req, res) {  
  res.render('index', {config,error:null})
});

app.listen(3508);

var chunks = "";

/* Run the sequence as a subprocess */
const seq = spawn('./sequence');

function resetChunks() {
  chunks = "";
  config.opCount += 1;
}

seq.stdout.on('data', (data) => {
  console.log(`stdout: ${data}`);

  chunks += data;

  if (chunks.includes("Input a word to have it decomposed")) {
    chunks = chunks.split("\n");
    config.lastWords = chunks.filter((str) => str.startsWith("- ")).map((str) => str.substr(2).trim());
    resetChunks();
  }
  if (chunks.trim().endsWith(")")) {
    console.log("decomposed");
    chunks = chunks.trim().split("\n").pop().trim();
    console.log(chunks);
    config.decomposed = chunks.substr(0, chunks.lastIndexOf(" "));
    resetChunks();
    console.log(config);
  }
});

seq.stderr.on('data', (data) => {
  console.log(`stderr: ${data}`);
});

seq.on('close', (code) => {
  console.log(`child process exited with code ${code}`);
});

function *exec(arg) {
  var count = config.opCount;
  seq.stdin.write(arg + "\n");

  while (config.opCount == count || config.status != "done") {
    yield delay(50);
  }
}

app.post("/", function(req, res) {
  console.log(JSON.stringify(req.body));

  co(function*() {
    var num = Number(req.body["number-seq"].trim());
    assert(Number.isInteger(num) && num > 0 && num < 60000, "The number provided must be an integer between 1 and 50000");

    yield exec(num);

    console.log("no errors!");

    config.number = num;
    res.render('index', {config, error:null});
  }).catch((err) => {
    console.log("sending back error", err);
    res.status(500);
    res.render('index', {config, error: err.message});
  });
});

app.post("/decompose", function(req, res) {
  console.log(JSON.stringify(req.body));

  co(function*() {
    var seq = req.body["sequence"].trim();
    
    if (seq.length == 0) {
      config.decomposed = "";
    } else {
      yield exec(seq);
    }
    res.send(config.decomposed).end();
  });
});
