const express = require('express');
const path = require('path');
const compression = require('compression');
const bodyParser = require('body-parser');

var config = {
  number: "",
  lastWords: []
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