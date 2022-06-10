console.log("Server Start - Nodejs - 123");

var express = require("express");
var app = express();

app.use(express.static('public'));
app.use('/images',express.static(__dirname + 'public/images'));

app.set("view engine","ejs");

app.set("views","./views"); 

var server = require("http").Server(app);

var io = require("socket.io").listen(server);

var myport = process.env.PORT || 3000;

server.listen(myport, function () {
    console.log('Server listening at port %d', myport);
});


io.on("connection", function(socket) 
{
	
  console.log("Có người truy cập id:" + socket.id);
  
	socket.on("disconnect",function()
	{
		
		console.log(socket.id + " ngắt kết nối server!!!!");
	});
	
	
		// lắng nghe html send data 
  socket.on("htmlclient", function(data) {
	  
    console.log("Html id:" + socket.id + " Sendata:" + data);
	// gửi xuống cho esp
	io.emit("ESP", data);	
  });
  
  
  // lắng nghe sự kiện app flutter
	socket.on("flutter", function(data) {
    console.log("AppFlutter id:" + socket.id + " Sendata:" + data);
	// gửi xuống cho esp
	io.emit("ESP", data);
  });
  
  
  // lắng nghe sự kiện ESP 
	socket.on("ESPJSON", function(data) 
	{
		var DataJson = JSON.stringify(data);
		
		console.log("Data ESP JSON: " + DataJson);
	
	// gửi  web
	io.emit("web", DataJson);
	// gửi app 
	io.emit("app", DataJson);
	
  });
	
  
});



app.get("/",function(req,res){
	//res.render("html");
	res.render("index");
});

