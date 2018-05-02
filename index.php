

<?php
	include('session.php');
?>
<!DOCTYPE html>
<html>
	<body class="dashboard">
		<link rel="stylesheet" href="/css/c3.css">
		<html lang="en">
		</html>
		<head>
			<meta charset="utf-8">
			<meta name="viewport" content="width=device-width, initial-scale=1">
			<meta name="theme-color" content="#2196F3">
			<link rel="stylesheet" href="/css/bootstrap.min.css">
			<script src="/js/jquery.min.js"></script>
			<script src="/js/bootstrap.min.js"></script>
			<style type="text/css">
			</style>
			<meta name="theme-color" content="#2196F3">
			<!-- Refresh every 15 minutes -->
		</head>
		<body>
			<nav class="navbar navbar-default navbar-static-top" role="navigation">
				<div class="container">
					<div class="navbar-header">
						<button type="button" class="navbar-toggle collapsed" data-toggle="collapse" data-target="#bs-example-navbar-collapse-1">
						<span class="sr-only">Toggle navigation</span>
						<span class="icon-bar"></span>
						<span class="icon-bar"></span>
						<span class="icon-bar"></span>
						</button>
					</div>
					<!-- Collect the nav links, forms, and other content for toggling -->
					<div class="collapse navbar-collapse" id="bs-example-navbar-collapse-1">
						<ul class="nav navbar-nav">
							<li><a href="index.php">Dashboard</a></li>
							<li><a href="hourly.php">Hourly Data</a></li>
							<li><a href="daily.php">Daily Data</a></li>
							<li><a href="weekly.php">Weekly Data</a></li>
						</ul>
						<ul class="nav navbar-nav pull-right">
							<li><a href="logout.php">Logout</a></li>
						</ul>
					</div>
			</nav>
			</div>
			<title>Weather Monitor</title>
			<div>
				<h2>
					<div id="rcorners2" style="background-color:#f9fbff"
			</div>
			<center> </center>
			</h2>
			<script>
				function GetClock(){
					var d = new Date();
					var days = ["Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"];
					var months = ["January","February","March","April","May","June","July"];
					document.getElementById("date").innerHTML = days[d.getDay()] + ", " + months[d.getMonth()] + " " + d.getDate() + ", " + d.getFullYear();
					nMin=d.getMinutes();
					if(nMin<=9) 
						nMin="0"+nMin
					nSec=d.getSeconds();
					if(nSec<=9) 
						nSec="0"+nSec
					nHour=d.getHours();
					if(nHour<=9) 
						nHour="0"+nHour
					document.getElementById("time").innerHTML = nHour +":"+ nMin+":"+ nSec;
				}
				window.onload=function(){
					GetClock();
					setInterval(GetClock,1000);
				}
								
			</script>
			<center>
				<div style="display: inline" id="date"></div>
			</center>
			<center>
				<div style="display: inline" id="time"></div>
			</center>
			<div style="white-space: pre-wrap;">&#10;</div>
			</div>
			<!--<div id="rcorners2" style="background-color:#f9fbff">-->
			<div id="chart"></div>
			</div>
			<table align="center" style="margin: 0px auto;">
				<tr>
					<h3>
						<center>Current Values</center>
					</h3>
					<td>
						<div style="width:30;"></div>
					</td>
					<td>
						<h4>
							<center>Temperature</center>
						</h4>
						<div id="temperature" style="width:220px; height:0px"></div>
					</td>
					<td>
						<div style="width:30;"></div>
					</td>
					<td>
						<h4>
							<center>Humidity</center>
						</h4>
						<div id="humidity" style="width: 220px;height:0px"></div>
					</td>
					<td>
						<div style="width:30;"></div>
					</td>
					<td>
						<h4>
							<center>Wind</center>
						</h4>
						<div id="wind" style="width: 220px;height:0px"></div>
					</td>
					<td>
						<div style="width:30; height:350px;"></div>
					</td>
				</tr>
			</table>
			</div>
			<script src="/js/d3.v3.min.js" charset="utf-8"></script>
			<script src="/js/c3.min.js" charset="utf-8"></script>
			<?php
				//open connection to mysql db
				$connection = mysqli_connect("192.168.43.79","root","root","weather3") or die("Error " . mysqli_error($connection));
				
				//fetch table rows from mysql db

				$sql = "select * from (select * from temp order by id desc limit 72) tmp order by tmp.id asc";
				$currentValues = "select * from (select * from temp order by id desc limit 1) tmp order by tmp.id asc";
				$result = mysqli_query($connection, $sql) or die("Error in Selecting " . mysqli_error($connection));
				$CVresult = mysqli_query($connection, $currentValues) or die("Error in Selecting " . mysqli_error($connection));
				
				//create an array
				$myArray = array();
				while($row =mysqli_fetch_assoc($result))
				{
					$myArray[] = $row;
				}
				$jsonString = json_encode($myArray);
				echo $row;
				//close the db connection
				
				
				//create an array
				$CVarray = array();
				while($row =mysqli_fetch_assoc($CVresult))
				{
					$CVarray[] = $row;
				}
				$CVvalueString = json_encode($CVarray);
				echo $row;
				//close the db connection
				mysqli_close($connection);
				?>
			<script>
				var myvar = <?php echo json_encode($myArray); ?>;
				var CVvalues = <?php echo json_encode($CVarray); ?>;
				
				
				// The variable name chart will automatically bind itself to an id = chart
				var chart = c3.generate({
					padding: {
						top: 10,
						right: 60,
						bottom: 10,
						left: 75,
					},
				
					data: {
						type : 'spline',
						types: {
							'StandardDeviation': 'bar',
        },
						colors: {
						'Temperature': '#ff0000',
						'Humidity': '#ffad33',
						'Wind': '#1e0bed',
						'StandardDeviation': 'green'
					},
						json: myvar,
						keys: {
							x: 'recorded',
							value: ['Temperature', 'Humidity', 'Wind', 'StandardDeviation']
						},
					selection: {
					//enabled: true,
					//multiple: true,
					draggable: true,
				  },
					},					
						zoom: {
					  enabled: true,
					  rescale: false,
					},
				    size: {
						height: 400
				    },
					   axis: {
						x: {
							type: 'category',
							tick: {
								outer: false,
								fit: false,
								show: false,
								rotate: -25,
								multiline: false,
								centered: true,	
							},
							height: 125
						},
					},
					grid: {
						x: {
							show: false
						},
						y: {
							show: true
						}
					},
									    tooltip: {
					        format: {
					            value: function (value, ratio, id) {
									if(id == "Temperature")
										return (value + " °C");
									
									else if(id == "Humidity")
										
										return (value + " %");

									else if(id == "Wind")
										return (value + " m/s");

									else if(id == "StandardDeviation")
										return (value + " m/s");
					            }
					        }
					    }
				
				});
				d3.select('#chart svg').append('text')
				 .attr('x', d3.select('#chart svg').node().getBoundingClientRect().width / 2)
				 .attr('y', 16)
				 .attr('text-anchor', 'middle')
				 .style('font-size', '20px')
				 .text('\nWeather Data');
				
				// The variable name chart will automatically bind itself to an id = chart
					c3.generate({
						bindto: '#temperature',
					    data: {
					        type : 'gauge',
							colors: {
							'Temperature': '#ff0000',
						},
					        json: CVvalues,
					        keys: {
					            x: 'ID',
					            value: ['Temperature']
					        },
					    },
						gauge: {
								label: {
								   format: function(value, ratio) {
										return value + "°C";
									},
									show: false // to turn off the min/max labels.
							   },
					    },
										    tooltip: {
					        format: {
					            value: function (value, ratio, id) {
					                return (value + " °C");
					            }
					        }
					    },
					
					});
					
					
					c3.generate({
						bindto: '#humidity',
					    data: {
					        type : 'gauge',
							colors: {
							'Humidity': '#ffad33',
						},
					        json: CVvalues,
					        keys: {
					            x: 'ID',
					            value: ['Humidity']
					        }
					
					    },
						gauge: {
								label: {
								   format: function(value, ratio) {
										return value + "%";
									},
									show: false // to turn off the min/max labels.
							   },
					    },
										    tooltip: {
					        format: {
					            value: function (value, ratio, id) {
					                return (value + " %");
					            }
					        }
					    }
					
					});
					
					c3.generate({
						bindto: '#wind',
					    data: {
					        type : 'gauge',
							colors: {
							'Wind': '#1e0bed',
						},
					        json: CVvalues,
					        keys: {
					            x: 'ID',
					            value: ['Wind']
					        }
						
					    },
						gauge: {
								label: {
								   format: function(value, ratio) {
										return value + "m/s";
									},
									show: false // to turn off the min/max labels.
							   },
					    },
					    tooltip: {
					        format: {
					            value: function (value, ratio, id) {
					                return (value + " m/s");
					            }
					        }
					    }
					
					});
					
			</script>
			<?php
				$filename = "http://api.sunrise-sunset.org/json?lat=53.270668&lng=-9.056791";
				$contents = file_get_contents($filename);
				$json = $contents; 
				$obj=json_decode($json);  
				
				$sunrise=$obj->results->sunrise;
				$sunset=$obj->results->sunset;
				$solarNoon=$obj->results->solar_noon;
				
				$sunrise = new DateTime("$sunrise +00");
				$sunset = new DateTime("$sunset +00");
				$solarNoon = new DateTime("$solarNoon +00");
				
				$sunrise->setTimezone(new DateTimeZone('Europe/Dublin'));
				$sunset->setTimezone(new DateTimeZone('Europe/Dublin'));
				$solarNoon->setTimezone(new DateTimeZone('Europe/Dublin'));
				
				?>
			<table align="center" height="250" style="margin: 10px auto;">
				<tr>
					<td>
						<div style="width:300; height:156px;"></div>
					</td>
					<td>
						<div class = "sunData" align="center">Sunrise</div>
						<img class = "sunData" src="sunrise.png" align="center" alt="sunrise" height="80" width="80">
						<div class = "sunData" align="center"><?php echo $sunrise->format('H:i'); ?></div>
					</td>
					<td>
						<div class = "sunData" align="center"> Solar Noon</div>
						<img class = "sunData" src="solarnoon.png" align="center" alt="solarnoon" height="80" width="80">
						<div class = "sunData" align="center"><?php echo $solarNoon->format('H:i'); ?></div>
					</td>
					<td>
						<div class = "sunData" align="center">Sunset</div>
						<img class = "sunData" src="sunset.png" align="center" alt="sunset" height="80" width="80">
						<div class = "sunData" align="center"><?php echo $sunset->format('H:i'); ?></div>
					</td>
				</tr>
			</table>
	</body>
</html>
