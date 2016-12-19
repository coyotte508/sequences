const express = require('express');
const path = require('path');
const compression = require('compression');
const bodyParser = require('body-parser');

const app = express();

app.use(compression());

app.use( bodyParser.json() );       // to support JSON-encoded bodies
app.use(bodyParser.urlencoded({     // to support URL-encoded bodies
  extended: true
})); 

console.log(__dirname + '/public');
app.use("/", express.static(__dirname + '/public'));
app.set('view engine', 'ejs'); 

app.get('/', function(req, res) {  
  res.render('index', {error:null})
});

app.listen(3508);