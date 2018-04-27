<?php
   include('session.php');
?>

<!DOCTYPE html>
<html>
<body class="hourly">
<link href="/css/c3.css" rel="stylesheet" type="text/css">
	<html lang="en">
	</html>
	<head>
		<meta http-equiv="refresh" content="180">
		<!-- Refresh every 15 minutes -->
	<meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="/css/bootstrap.min.css">
  <script src="/js/jquery.min.js"></script>
  <script src="/js/bootstrap.min.js"></script>
	</head>
	<body>
	<title>Daily Weather Statistics</title>
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
</body>
	<div>
		<h2>
			<center>Daily Statistics</center>
		</h2>
	</div>
	<body>
		<td>
			<div style="width:30; height:10px;"></div>
		</td>
		<td>
			<div id="chart"></div>
		</td>
		<td>
			<div style="width:30; height:10px;"></div>
		</td>

		<table align="center" style="margin: 0px auto;">
			<tr>
				<h3>
					<center> Daily Average Values</center>
				</h3>
				<td>
					<div style="width:30; height:5px;"></div>
				</td>
				<td>
					<h4><center>Temperature</center></h4>
					<div id="temperatureAVG" style="width:220px;"></div>
				</td>
				<td>
					<div style="width:30;"></div>
				</td>
				<td>
				<h4><center>Humidity</center></h4>
					<div id="humidityAVG" style="width:220px;"></div>
				</td>
				<td>
					<div style="width:30;"></div>
				</td>
				<td>
				<h4><center>Wind</center></h4>
					<div id="windAVG" style="width:220px;"></div>
				</td>
				<td>
					<div style="width:30; height:300px;"></div>
				</td>
			</tr>
		</table>
		<div style="white-space: pre-wrap;">&#10;</div>
		<div style="white-space: pre-wrap;">&#10;</div>
		<div style="white-space: pre-wrap;">&#10;</div>
				<table align="center" style="margin: 0px auto;">
			<tr>
				<h3>
					<center> Daily Maximum Values</center>
				</h3>
				<td>
					<h4><center>Temperature Maximum</center></h4>
					<div id="temperatureMax" style="width:220px;"></div>
				</td>
				<td>
					<div style="width:30;"></div>
				</td>
				<td>
				<h4><center>Humidity Maximum</center></h4>
					<div id="humidityMax" style="width:220px;"></div>
				</td>
				<td>
					<div style="width:30;"></div>
				</td>
				<td>
				<h4><center>Wind Maximum</center></h4>
					<div id="windMax" style="width:220px;"></div>
				</td>
				<td>
					<div style="width:30; height:300px;"></div>
				</td>
			</tr>
		</table>
		<div style="white-space: pre-wrap;">&#10;</div>
		<div style="white-space: pre-wrap;">&#10;</div>
		<div style="white-space: pre-wrap;">&#10;</div>
						<table align="center" style="margin: 0px auto;">
			<tr>
				<h3>
					<center> Daily Minimum Values</center>
				</h3>
				<td>
					<div style="width:30; height:5px;"></div>
				</td>
				<td>
					<h4><center>Temperature Minimum</center></h4>
					<div id="temperatureMin" style="width:220px;"></div>
				</td>
				<td>
					<div style="width:30;"></div>
				</td>
				<td>
				<h4><center>Humidity Minimum</center></h4>
					<div id="humidityMin" style="width:220px;"></div>
				</td>
				<td>
					<div style="width:30;"></div>
				</td>
				<td>
				<h4><center>Wind Minimum</center></h4>
					<div id="windMin" style="width:220px;"></div>
				</td>
				<td>
					<div style="width:30; height:300px;"></div>
				</td>
			</tr>
		</table>
		<script>document.addEventListener('touchstart', onTouchStart, {passive: true});</script>
		<link href="/css/c3.css" rel="stylesheet" type="text/css">
		<script src="/js/d3.v3.min.js" charset="utf-8"></script>
		<script src="/js/c3.min.js" charset="utf-8"></script>
		
		<?php
			//open connection to mysql db
			$connection = mysqli_connect("192.168.43.79","root","root","weather3") or die("Error " . mysqli_error($connection));
			
			
			//fetch table rows from mysql db
			
			$dailyAverageLine = "select * from (select * from dailyavg order by id desc limit 30) tmp order by tmp.id asc";
			$dailyAverage = "SELECT * FROM (select * from dailyavg order by ID desc limit 1) tmp order by tmp.ID asc";
			$dailyMax = "SELECT * FROM (select * from dailymax order by ID desc limit 1) tmp order by tmp.ID asc";
			$dailyMin = "SELECT * FROM (select * from dailymin order by ID desc limit 1	) tmp order by tmp.ID asc";
			
			$result = mysqli_query($connection, $dailyAverageLine) or die("Error in Selecting " . mysqli_error($connection));
			$dailyAvgResult = mysqli_query($connection, $dailyAverage) or die("Error in Selecting " . mysqli_error($connection));
			$dailyMaxResult = mysqli_query($connection, $dailyMax) or die("Error in Selecting " . mysqli_error($connection));
			$dailyMinResult = mysqli_query($connection, $dailyMin) or die("Error in Selecting " . mysqli_error($connection));
			
			//create an array
			$DavgLineArray = array();
			while($row =mysqli_fetch_assoc($result))
			{
			    $DavgLineArray[] = $row;
			}
			$jsonString = json_encode($DavgLineArray);
			
			
			//create an array
			$dailyAvgArray = array();
			while($row1 =mysqli_fetch_assoc($dailyAvgResult))
			{
			    $dailyAvgArray[] = $row1;
			}
			$jsonString1 = json_encode($dailyAvgArray);
			
			//create an array
			$dailyMaxArray = array();
			while($row1 =mysqli_fetch_assoc($dailyMaxResult))
			{
			    $dailyMaxArray[] = $row1;
			}
			$jsonString2 = json_encode($dailyMaxArray);
			
			//create an array
			$dailyMinArray = array();
			while($row1 =mysqli_fetch_assoc($dailyMinResult))
			{
			    $dailyMinArray[] = $row1;
			}
			$jsonString3 = json_encode($dailyMinArray);
			
			
			//close the db connection
			mysqli_close($connection);
			?>
		<div>
			<script>
				var dLineAvg = <?php echo json_encode($DavgLineArray); ?>;
				var dAvg = <?php echo json_encode($dailyAvgArray); ?>;
				var dMax = <?php echo json_encode($dailyMaxArray); ?>;
				var dMin = <?php echo json_encode($dailyMinArray); ?>;
				
				c3.generate({
					bindto: '#chart',
					padding: {
				        top: 10,
				        right: 75,
				        bottom: 10,
				        left: 110,
				    },
				    data: {
				        type : 'spline',
						colors: {
					'Temperature': '#ff0000',
					'Humidity': '#ffad33',
					'Wind': '#1e0bed'
					},
				        json: dLineAvg,
				        keys: {
				            x: 'time1',
				            value: ['Temperature', 'Humidity', 'Wind']
				        },selection: {

			  },

				},
				
					zoom: {
				  enabled: true,
				  rescale: true,
				},
			    size: {
					height: 400
			    },
				   axis: {
					x: {
						type: 'category',
						tick: {
							outer: false,
							//values: [5,10,15,20,25,30,35,40,45,],
							show: false,
							rotate: -25,
							multiline: false,
							centered: true,
							culling: {
								max:18 // the number of tick texts will be adjusted to less than this value
							},
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
								else
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
    .text('Daily Weather Data');
					

				c3.generate({
					bindto: '#temperatureAVG',
				    data: {
				        type : 'gauge',
						colors: {
						'Temperature': '#ff0000',
					},
				        json: dAvg,
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
					bindto: '#humidityAVG',
				    data: {
				        type : 'gauge',
						colors: {
						'Humidity': '#ffad33',
					},
				        json: dAvg,
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
					bindto: '#windAVG',
				    data: {
				        type : 'gauge',
						colors: {
						'Wind': '#1e0bed',
					},
				        json: dAvg,
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
								// The variable name chart will automatically bind itself to an id = chart
				c3.generate({
					bindto: '#temperatureMax',
				    data: {
				        type : 'gauge',
						colors: {
						'Temperature': '#ff0000',
					},
				        json: dMax,
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
					bindto: '#humidityMax',
				    data: {
				        type : 'gauge',
						colors: {
						'Humidity': '#ffad33',
					},
				        json: dMax,
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
					bindto: '#windMax',
				    data: {
				        type : 'gauge',
						colors: {
						'Wind': '#1e0bed',
					},
				        json: dMax,
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
				
								c3.generate({
					bindto: '#temperatureMin',
				    data: {
				        type : 'gauge',
						colors: {
						'Temperature': '#ff0000',
					},
				        json: dMin,
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
					bindto: '#humidityMin',
				    data: {
				        type : 'gauge',
						colors: {
						'Humidity': '#ffad33',
					},
				        json: dMin,
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
					bindto: '#windMin',
				    data: {
				        type : 'gauge',
						colors: {
						'Wind': '#1e0bed',
					},
				        json: dMin,
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
			<td>
				<div style="width:30; height:500px;"></div>
			</td>
			<center></center>
			</script>
		</div>
</body>
</html>

